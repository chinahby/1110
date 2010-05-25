/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    R F C O M M    R E C E I V E

GENERAL DESCRIPTION
  This file contains the Bluetooth code that processes received RFCOMM
  messages.

Copyright (c) 2000 - 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrcrx.c_v   1.16   24 Jul 2002 12:06:58   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrcrx.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrcrx.c_v  $
 *
 *    #26        29 May 2007            PR
 * Cleanup when p2p timer expires
 *
 *    #25        02 May 2007            SSK
 * Remove unused variables to fix compiler warnings. 
 *
 *    #24        21 Feb 2007            PN
 * Fixed critical lint errors.
 *
 *    #23        11 Aug 2006            GB
 * Modified to support APIT mode in ASIA
 *
 *    #22        25 May 2006            JTL
 * Fixing possible bug with error messages & dereferencing NULL ptrs.
 *
 *    #20-21     18 May 2006            JTL
 * Calculate optimal MFS given a small watermark DNE. Tries to limit
 * MFS to a baseband-sized chunk smaller than the DNE.
 *
 *    #19        17 Apr 2006            JTL
 * Bring down DLC after rejecting due to security reasons. This will cause
 * RC to bring down the MCC if there are no other DLCs.
 *
 *    #18        06 Dec 2005            DSN
 * Fixed RVCT compile errors.
 *
 *    #17        1  Dec 2005            DP
 * Changed CBFC to support cases where there is no RX watermark by only
 * allowing one outstanding transmit credit at a time to the remote side.
 * This is known to apply for Circuit Switched (CS) data calls.
 *
 *    #16        14 Jul 2005            DP
 * Do not require receipt of MSC Response to enter connected state.
 *
 *    #15        16 Jun 2005            DP
 * Fixed race condition for disconnecting.
 *
 *    #14        8  Apr 2005            DP
 * Make use of BT_BDA debug message macro.
 *
 *    #13        7  Apr 2005            DP
 * Process DM messages from the remote side more broadly to increase
 * interoperability.  They are now processed in cases when they are sent
 * at times disallowed by the specification.
 *
 *    #12        16 Mar 2005            DP
 * Fix Mode 2 security for client connections.
 *
 *    #11        8  Mar 2005            DP
 * Improved CBFC so it now uses actual space available in receive watermark
 * to determine number of transmit credits to give remote device.
 *
 *    #10        1  Jun 2004            DP
 * Implemented Mode 2 security.
 *
 *    #9         19 Dec 2003            DP
 * Improved disconnect and connection failure indications to applications.
 *
 *    #8         14 Nov 2003            DP
 * Send connection failed event if DM received on MCC bring up.
 *
 *    #6         4  Nov 2003            DP
 * General cleanup.
 *
 *    #5         26 Sep 2003            DP
 * Fixed corner case when received bad MCC header length.
 * 
 *    Rev 1.16   24 Jul 2002 12:06:58   propach
 * Improved SIO API for SPP.
 * 
 *    Rev 1.15   03 Apr 2002 14:06:40   propach
 * Enhanced modem status changed events.
 * 
 *    Rev 1.14   21 Feb 2002 19:17:50   ryuen
 * Fixed compiler warning when FEATURE_BT_SIO_API_ENHANCEMENT is defined
 * 
 *    Rev 1.13   08 Feb 2002 17:40:06   ryuen
 * Updated bt_rc_rx_process_uih to call bt_sio_process_rc_rx_data if
 * FEATURE_BT_SIO_API_ENHANCEMENT is defined
 * 
 *    Rev 1.12   23 Oct 2001 16:52:10   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>

#include "bt.h"
#include "bti.h"
#include "btmsg.h"
#include "btrc.h"
#include "btrcev.h"
#include "btrci.h"
#include "btrcmsgi.h"
#include "btrcrx.h"
#include "btrctx.h"
#include "btsio.h"
#include "btutils.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_MSG_LAYER BT_MSG_RC


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

#ifdef BT_SWDEV_QXDM_LOG_MSG
/*===========================================================================

FUNCTION
  bt_rc_send_rx_signaling_log

DESCRIPTION
  Logs all the bytes in a specified received RFCOMM signaling packet
  to the QXDM.

===========================================================================*/
LOCAL void bt_rc_send_rx_signaling_log
(
  dsm_item_type*  dsm_ptr,
  uint16          cid
)
{

  log_bt_rc_sig_rx_type*  log_ptr;
  uint32                  data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_RC_LOG_BUFFER_SIZE );

  log_ptr = (log_bt_rc_sig_rx_type *)log_alloc( log_bt_rc_sig_rx,
                       sizeof( log_bt_rc_sig_rx_type ) -
                       BT_RC_LOG_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length    = data_length;
    log_ptr->dlci      = 0x0;  /*  Can't know yet - not parsed.  */
    log_ptr->local_cid = cid; 

    dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                 (void *)log_ptr->buffer,
                 data_length );

    log_commit( log_ptr );
  }

}

/*===========================================================================

FUNCTION
  bt_rc_send_payload_rx_log

DESCRIPTION
  Logs the first and last bytes in a specified received RFCOMM
  payload packet to the QXDM.

===========================================================================*/
LOCAL void bt_rc_send_payload_rx_log
(
  dsm_item_type   *dsm_ptr,
  uint8           dlci,
  uint16          cid
)
{
  log_bt_rc_pyld_rx_type*  log_ptr;
  uint32                   data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_PYLD_BUFFER_SIZE );
  
  log_ptr = (log_bt_rc_pyld_rx_type *)log_alloc( log_bt_rc_pyld_rx,
                       sizeof( log_bt_rc_pyld_rx_type ) -
                       BT_PYLD_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length    = data_length;
    log_ptr->dlci      = dlci;
    log_ptr->local_cid = cid;

    dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                 (void *)(log_ptr->buffer), 
                 MIN( data_length, BT_FIRST_TEN_PYLD_BYTES ) );

    if ( data_length >=
           (BT_FIRST_TEN_PYLD_BYTES + BT_LAST_TEN_PYLD_BYTES) )
    {   
      dsm_extract(
        dsm_ptr, (data_length - BT_LAST_TEN_PYLD_BYTES),
        (void *)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
        BT_LAST_TEN_PYLD_BYTES );
    }
    else if ( data_length > BT_FIRST_TEN_PYLD_BYTES )
    {   
      dsm_extract(
        dsm_ptr, BT_FIRST_TEN_PYLD_BYTES,
        (void *)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
        (data_length - BT_FIRST_TEN_PYLD_BYTES) );
    }
    log_commit( log_ptr );
  }

}
#endif /* BT_SWDEV_QXDM_LOG_MSG */

/*===========================================================================

FUNCTION
  bt_rc_rx_sabm_service_sec_result

DESCRIPTION
  Processes service security results initiated by the receipt of an
  SABM frame.

===========================================================================*/
void bt_rc_rx_sabm_service_sec_result
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  boolean              secure_enough
)
{

  if ( secure_enough != FALSE )
  {
    bt_rc_tx_ua_frame( ssn_ptr, dlc_ptr );

    dlc_ptr->expected_rx_mask = BT_ERXM_MSC_CMD_B;
    bt_rc_tx_msc_command( ssn_ptr, dlc_ptr );
  }
  else
  {
    /*  Connection does not meet security requirements;  send DM.  */
    bt_rc_tx_dm_frame( ssn_ptr, dlc_ptr->dlci );
    
    BT_MSG_HIGH( "BT RC: Serv Sec forces DM PID %x SCN %x",
                 dlc_ptr->pid, dlc_ptr->server_channel, 0 );
    BT_BDA( MSG_HIGH, "BT RC: Serv Sec forces DM", &ssn_ptr->bd_addr );

    /* Insure MCC gets taken down if necessary, and event sent to app */
    bt_rc_bring_down_dlc( ssn_ptr, dlc_ptr, TRUE, BT_EVR_RM_SECURITY_NOT_MET );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_process_sabm

DESCRIPTION
  Processes a received SABM frame.

===========================================================================*/
LOCAL void bt_rc_rx_process_sabm
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  BT_MSG_SIG( "BT RC: RX SABM DLCI %x CID %x",
              rxi_ptr->dlc_ptr->dlci, rxi_ptr->ssn_ptr->cid, 0 );

  /*  SABM frames with P=0 shall be discarded.  */
  if ( rxi_ptr->fh_poll_final != FALSE )
  {
    if ( (rxi_ptr->dlc_ptr->state == BT_RCDS_CONNECTING_DLC) &&
         (rxi_ptr->dlc_ptr->expected_rx_mask == BT_ERXM_SABM_B) )
    {
      rxi_ptr->dlc_ptr->expected_rx_mask = 0;

      if ( rxi_ptr->fh_dlci == RC_DLCI_MCC_V )
      {
        bt_rc_tx_ua_frame( rxi_ptr->ssn_ptr, rxi_ptr->dlc_ptr );
        rxi_ptr->dlc_ptr->state = BT_RCDS_CONNECTED;
      }
      else
      {
        BT_MSG_API( "BT RC CMD TX: RM Enf Sec SCN %x",
                    rxi_ptr->dlc_ptr->server_channel, 0, 0 );
        BT_BDA( MSG_API, "BT RC CMD TX: RM Enf Sec SCN",
                &rxi_ptr->ssn_ptr->bd_addr );
        bt_cmd_rm_enforce_security_rfcomm_scn(
          bt_rc_app_id, rxi_ptr->dlc_ptr->server_channel,
          &rxi_ptr->ssn_ptr->bd_addr, FALSE );
      }
    }
    else
    {
      /*  Unexpected SABM received or bad DLC state. */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Bad SABM DLCI %x ERXM %x St %x",
              rxi_ptr->dlc_ptr->dlci,
              rxi_ptr->dlc_ptr->expected_rx_mask,
              rxi_ptr->dlc_ptr->state );
    }
  }
  else
  {
    /*  P=0 unexpected in SABM.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad P=0 SABM DLCI %x P %x",
            rxi_ptr->dlc_ptr->dlci, rxi_ptr->fh_poll_final, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_process_ua

DESCRIPTION
  Processes a received UA frame.

===========================================================================*/
LOCAL void bt_rc_rx_process_ua
(
  bt_rc_rx_info_type*  rxi_ptr
)
{
  struct dlc_struct* temp_dlc_ptr;

  BT_MSG_SIG( "BT RC: RX UA DLCI %x CID %x",
              rxi_ptr->dlc_ptr->dlci, rxi_ptr->ssn_ptr->cid, 0 );

  /*  UA frames with F=0 shall be discarded.  */
  if ( rxi_ptr->fh_poll_final != FALSE )
  {
    if ( rxi_ptr->dlc_ptr->expected_rx_mask & BT_ERXM_UA_OR_DM_B )
    {
      rxi_ptr->dlc_ptr->expected_rx_mask &= ~BT_ERXM_UA_OR_DM_B;
      if ( rxi_ptr->dlc_ptr->state == BT_RCDS_CONNECTING_DLC )
      {
        if ( rxi_ptr->fh_dlci == RC_DLCI_MCC_V )
        {
          rxi_ptr->dlc_ptr->state = BT_RCDS_CONNECTED;

          temp_dlc_ptr = bt_rc_find_free_dlc_entry( rxi_ptr->ssn_ptr );

          if ( temp_dlc_ptr != NULL )
          {
            rxi_ptr->dlc_ptr = temp_dlc_ptr;
            rxi_ptr->dlc_ptr->dlci = BT_DLC_SC_2_DLCI(
                       rxi_ptr->ssn_ptr->mcc.server_channel );
            if ( rxi_ptr->ssn_ptr->mcc.direction != FALSE )
            {
              rxi_ptr->dlc_ptr->dlci |= BT_DLC_DLCI_DIR_B;
            }
            rxi_ptr->dlc_ptr->direction = rxi_ptr->ssn_ptr->mcc.direction;
            rxi_ptr->dlc_ptr->server_channel =
                                rxi_ptr->ssn_ptr->mcc.server_channel;
            rxi_ptr->dlc_ptr->bt_app_id = rxi_ptr->ssn_ptr->mcc.bt_app_id;
            rxi_ptr->dlc_ptr->tx_wm_ptr = rxi_ptr->ssn_ptr->mcc.tx_wm_ptr;
            rxi_ptr->dlc_ptr->rx_wm_ptr = rxi_ptr->ssn_ptr->mcc.rx_wm_ptr;

            bt_rc_set_local_client_dlc_config( rxi_ptr->ssn_ptr,
                                               rxi_ptr->dlc_ptr );

            rxi_ptr->dlc_ptr->state = BT_RCDS_CONNECTING_DLC;

            rxi_ptr->ssn_ptr->mcc.direction      = 0xF0;
            rxi_ptr->ssn_ptr->mcc.server_channel = BT_RC_SC_NULL;
            rxi_ptr->ssn_ptr->mcc.bt_app_id      = BT_APP_ID_NULL;
            rxi_ptr->ssn_ptr->mcc.tx_wm_ptr      = NULL;
            rxi_ptr->ssn_ptr->mcc.rx_wm_ptr      = NULL;

            rxi_ptr->ssn_ptr->dlc_count++;

            bt_rc_bring_up_local_client_dlc( rxi_ptr->ssn_ptr,
                                             rxi_ptr->dlc_ptr );
          }
          else
          {
            /*  No room for new DLC.  */
            rxi_ptr->error = TRUE;
            BT_ERR( "BT RC: Bad UA no room DLCI %x Cnt %x",
                    rxi_ptr->dlc_ptr->dlci,
                    rxi_ptr->ssn_ptr->dlc_count, 0 );
          }
        }
        else
        {
          rxi_ptr->dlc_ptr->expected_rx_mask = BT_ERXM_MSC_CMD_B;
          bt_rc_tx_msc_command( rxi_ptr->ssn_ptr, rxi_ptr->dlc_ptr );
        }
      }
      else if ( rxi_ptr->dlc_ptr->state == BT_RCDS_DISCONNECTING_DLC )
      {
        rxi_ptr->error = bt_rc_bring_down_dlc(
                           rxi_ptr->ssn_ptr, rxi_ptr->dlc_ptr,
                           FALSE, BT_EVR_RC_NORMAL_DISCONNECT );
      }
      else
      {
        /*  Bad DLC state.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad UA DLC St %x ERXM %x PID %x",
                rxi_ptr->dlc_ptr->state,
                rxi_ptr->dlc_ptr->expected_rx_mask,
                rxi_ptr->dlc_ptr->pid );
      }
    }
    else
    {
      /*  Unexpected UA received.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Bad UA ERXM %x St %x PID %x",
              rxi_ptr->dlc_ptr->expected_rx_mask,
              rxi_ptr->dlc_ptr->state,
              rxi_ptr->dlc_ptr->pid );
    }
  }
  else
  {
    /*  F=0 unexpected in UA.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad F=0 UA DLCI %x F %x",
            rxi_ptr->dlc_ptr->dlci, rxi_ptr->fh_poll_final, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_process_disc

DESCRIPTION
  Processes a received DISC frame.

===========================================================================*/
LOCAL void bt_rc_rx_process_disc
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8    i;
  boolean  conn_failed;

  BT_MSG_SIG( "BT RC: RX DISC DLCI %x CID %x",
              rxi_ptr->dlc_ptr->dlci, rxi_ptr->ssn_ptr->cid, 0 );

  /*  DISC frames with P=0 shall be discarded.  */
  if ( rxi_ptr->fh_poll_final != FALSE )
  {
    bt_rc_tx_ua_frame( rxi_ptr->ssn_ptr, rxi_ptr->dlc_ptr );

    if ( rxi_ptr->fh_dlci != RC_DLCI_MCC_V )
    {
      if ( rxi_ptr->dlc_ptr->state == BT_RCDS_CONNECTING_DLC )
      {
        conn_failed = TRUE;
      }
      else
      {
        conn_failed = FALSE;
      }

      if ( rxi_ptr->dlc_ptr->state != BT_RCDS_DISCONNECTING_DLC )
      {
        rxi_ptr->dlc_ptr->state = BT_RCDS_DISCONNECTING_DLC;

        rxi_ptr->error = bt_rc_update_disconnected_dlc(
                           rxi_ptr->ssn_ptr, rxi_ptr->dlc_ptr,
                           conn_failed, BT_EVR_RC_NORMAL_DISCONNECT );
      }
    }
    else
    {
      if ( rxi_ptr->dlc_ptr->state != BT_RCDS_DISCONNECTING_DLC )
      {
        if ( (rxi_ptr->ssn_ptr->mcc.state == BT_RCDS_CONNECTING_DLC) &&
             (rxi_ptr->ssn_ptr->dlc_count == 0) )
        {
          /*  Was bringing up MCC for first DLCI.        */
          /*  Inform application of connection failure.  */
          bt_rc_ev_send_connection_failed(
            rxi_ptr->ssn_ptr->mcc.bt_app_id,
            &rxi_ptr->ssn_ptr->bd_addr, BT_EVR_RC_NORMAL_DISCONNECT );
        }
        else if ( rxi_ptr->ssn_ptr->dlc_count != 0 )
        {
          /*  Disconnected Multiplexer with open DLCs.  */
          BT_VSTAT_INC( RC_L2_DISC_DLCS_ACTIVE );

          for ( i = 0; i < BT_RC_MAX_PORTS_PER_SESSION; i++ )
          {
            if ( rxi_ptr->ssn_ptr->dlc[ i ].state != BT_RCDS_DISCONNECTED )
            {
              if ( rxi_ptr->ssn_ptr->dlc[ i ].state ==
                     BT_RCDS_CONNECTING_DLC )
              {
                conn_failed = TRUE;
              }
              else
              {
                conn_failed = FALSE;
              }
              rxi_ptr->ssn_ptr->dlc[ i ].state = BT_RCDS_DISCONNECTING_DLC;
              bt_rc_update_disconnected_dlc(
                rxi_ptr->ssn_ptr, &(rxi_ptr->ssn_ptr->dlc[ i ]),
                conn_failed, BT_EVR_RC_NORMAL_DISCONNECT );
            }
          }
        }

        rxi_ptr->ssn_ptr->mcc.state = BT_RCDS_DISCONNECTING_DLC;
        rxi_ptr->error = bt_rc_update_disconnected_mcc(
                           rxi_ptr->ssn_ptr );

        if ( rxi_ptr->ssn_ptr->mcc.state == BT_RCDS_DISCONNECTING_L2CAP )
        {
           rxi_ptr->ssn_ptr->l2_disc_wait = TRUE;
        }
      }
    }
  }
  else
  {
    /*  P=0 unexpected in DISC.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad P=0 DISC DLCI %x P %x",
            rxi_ptr->dlc_ptr->dlci, rxi_ptr->fh_poll_final, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_process_dm

DESCRIPTION
  Processes a received DM frame.

===========================================================================*/
LOCAL void bt_rc_rx_process_dm
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  BT_MSG_SIG( "BT RC: RX DM DLCI %x CID %x",
              rxi_ptr->dlc_ptr->dlci, rxi_ptr->ssn_ptr->cid, 0 );
  
  if ( ((rxi_ptr->dlc_ptr->expected_rx_mask & BT_ERXM_UA_OR_DM_B) == 0) ||
       (rxi_ptr->dlc_ptr->state != BT_RCDS_CONNECTING_DLC) )
  {
    /*  Unexpected DM received.  */
    BT_ERR( "BT RC: Bad DM DLC ERXM %x St %x PID %x",
            rxi_ptr->dlc_ptr->expected_rx_mask,
            rxi_ptr->dlc_ptr->state, rxi_ptr->dlc_ptr->pid );
  }

  rxi_ptr->dlc_ptr->expected_rx_mask &= ~BT_ERXM_UA_OR_DM_B;
  rxi_ptr->dlc_ptr->state = BT_RCDS_DISCONNECTING_DLC;
  
  if ( rxi_ptr->fh_dlci == RC_DLCI_MCC_V )
  {
    bt_rc_ev_send_connection_failed(
      rxi_ptr->dlc_ptr->bt_app_id,
      &(rxi_ptr->ssn_ptr->bd_addr),
      BT_EVR_RC_REMOTE_REJECTS_CONNECTION );
  }

  rxi_ptr->error = bt_rc_bring_down_dlc(
                     rxi_ptr->ssn_ptr, rxi_ptr->dlc_ptr,
                     TRUE, BT_EVR_RC_REMOTE_REJECTS_CONNECTION );

}


/*===========================================================================

FUNCTION
  bt_rc_rx_unpack_mcc_hdr

DESCRIPTION
  Unpacks a received MCC header into the passed rx_info structure.

===========================================================================*/
LOCAL void bt_rc_rx_unpack_mcc_hdr
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  bt_rc_mcc_hdre_type  mcc_hdr;

  if ( (dsm_pullup( &rxi_ptr->dsm_ptr, (void *)(&mcc_hdr),
                    sizeof( bt_rc_mcc_hdr_type ))) ==
        sizeof( bt_rc_mcc_hdr_type ) )
  {
    if ( mcc_hdr.type & RC_CR_CMD_B )
    {
      rxi_ptr->mcch_command = TRUE;
    }
    else
    {
      rxi_ptr->mcch_command = FALSE;
    }
    rxi_ptr->mcch_type = (mcc_hdr.type & RCMCCH_TYPE_MT_M);
    rxi_ptr->mcch_length = ((mcc_hdr.length1 & RCMCCH_LEN_M) >> 1);
    if ( !(mcc_hdr.length1 & RC_EA_LAST_B) )
    {
      if ( (dsm_pullup( &rxi_ptr->dsm_ptr,
                        (void *)&mcc_hdr.length2,
                        sizeof( uint8 ) )) == sizeof( uint8 ) )
      {
        rxi_ptr->mcch_length |= ((mcc_hdr.length2 & RCMCCH_LEN_M) << 6);
        while ( ((rxi_ptr->error == FALSE) &&
                 (!(mcc_hdr.length2 & RC_EA_LAST_B))) )
        {
          BT_ERR( "BT RC: MCC vehdr Len %x %x DLCI %x",
                  rxi_ptr->mcch_length, mcc_hdr.length2,
                  rxi_ptr->dlc_ptr->dlci );

          if ( (dsm_pullup( &rxi_ptr->dsm_ptr,
                            (void *)&mcc_hdr.length2,
                            sizeof( uint8 ) )) == sizeof( uint8 ) )
          {
            if ( (mcc_hdr.length2 & RCMCCH_LEN_M) != 0 )
            {
              /*  This MCC header length is extended beyond reason. */
              rxi_ptr->error = TRUE;
              BT_ERR( "BT RC: MCC vehdr bad Len %x %x DLCI %x",
                      rxi_ptr->mcch_length, mcc_hdr.length2,
                      rxi_ptr->dlc_ptr->dlci );
            }
          }
          else
          {
            /*  Not enough data in received MCC header      */
            /*  for very extended MCC header length field.  */
            rxi_ptr->error = TRUE;
            BT_ERR( "BT RC: MCC vehdr short Len %x %x DLCI %x",
                    rxi_ptr->mcch_length, mcc_hdr.length2,
                    rxi_ptr->dlc_ptr->dlci );
          }
        }
      }
      else
      {
        /*  Not enough data in received MCC header  */
        /*  for extended MCC header length field.   */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad MCC ehdr short Len %x %x DLCI %x",
                rxi_ptr->mcch_length, mcc_hdr.length1,
                rxi_ptr->dlc_ptr->dlci );
      }
    }
  }
  else
  {
    /*  Not enough data for MCC header.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad MCC hdr short DLCI %x",
            rxi_ptr->dlc_ptr->dlci, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_test_command

DESCRIPTION
  Processes a Test command message received on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_test_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  BT_MSG_SIG( "BT RC: RX Test Cmd Len %x CID %x",
              rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );

  rxi_ptr->ssn_ptr->test_cmds_rxd++;

  bt_rc_tx_mcc_test_response( rxi_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_test_response

DESCRIPTION
  Processes a Test command response message received on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_test_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint16          i = 0;
  uint8           exp_value;
  uint8           rx_value;
  bt_ev_msg_type  ev_rc_pr;

  if ( rxi_ptr->ssn_ptr->mcc.expected_rx_mask & BT_ERXM_TEST_RESP_B )
  {
    BT_MSG_SIG( "BT RC: RX Test Resp Len %x Sidx %x CID %x",
                rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->ssn_idx,
                rxi_ptr->ssn_ptr->cid );
    
    rxi_ptr->ssn_ptr->mcc.expected_rx_mask &= ~BT_ERXM_TEST_RESP_B;

    rxi_ptr->ssn_ptr->test_resps_rxd++;

    ev_rc_pr.ev_hdr.ev_type = BT_EV_RC_PING_RESULT;
    ev_rc_pr.ev_hdr.bt_app_id =
      rxi_ptr->ssn_ptr->last_test_tx_bt_app_id;
    ev_rc_pr.ev_msg.ev_rc_ping_result.pid =
      rxi_ptr->ssn_ptr->last_test_tx_pid;
    ev_rc_pr.ev_msg.ev_rc_ping_result.success = TRUE;
    ev_rc_pr.ev_msg.ev_rc_ping_result.byte_count =
      rxi_ptr->ssn_ptr->last_test_tx_byte_count;
    ev_rc_pr.ev_msg.ev_rc_ping_result.start_value =
      rxi_ptr->ssn_ptr->last_test_tx_start_value;
    ev_rc_pr.ev_msg.ev_rc_ping_result.increment =
      rxi_ptr->ssn_ptr->last_test_tx_increment;
    ev_rc_pr.ev_msg.ev_rc_ping_result.rxd_byte_count =
      rxi_ptr->mcch_length;
    ev_rc_pr.ev_msg.ev_rc_ping_result.bad_value = FALSE;
    ev_rc_pr.ev_msg.ev_rc_ping_result.bad_value_idx = 0;
    ev_rc_pr.ev_msg.ev_rc_ping_result.exp_value = 0;
    ev_rc_pr.ev_msg.ev_rc_ping_result.rxd_value = 0;

    if ( rxi_ptr->mcch_length ==
           rxi_ptr->ssn_ptr->last_test_tx_byte_count )
    {
      exp_value = rxi_ptr->ssn_ptr->last_test_tx_start_value;
      while ( (i < rxi_ptr->ssn_ptr->last_test_tx_byte_count) &&
              (rxi_ptr->error == FALSE) )
      {
        rx_value = dsm_pull8( &rxi_ptr->dsm_ptr );
        if ( exp_value != rx_value )
        {
          ev_rc_pr.ev_msg.ev_rc_ping_result.bad_value = TRUE;
          ev_rc_pr.ev_msg.ev_rc_ping_result.bad_value_idx = i;
          ev_rc_pr.ev_msg.ev_rc_ping_result.exp_value = exp_value;
          ev_rc_pr.ev_msg.ev_rc_ping_result.rxd_value = rx_value;
          
          rxi_ptr->ssn_ptr->test_errors++;
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Bad MCC Test Resp %02x %02x %x",
                  exp_value, rx_value, i );
        }
        exp_value += rxi_ptr->ssn_ptr->last_test_tx_increment;
        i++;
      }
    }
    else
    {
      ev_rc_pr.ev_msg.ev_rc_ping_result.success = FALSE;
      rxi_ptr->ssn_ptr->test_errors++;
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Bad MCC Test Resp Len %x %x Sidx %x",
              rxi_ptr->mcch_length,
              rxi_ptr->ssn_ptr->last_test_tx_byte_count,
              rxi_ptr->ssn_ptr->ssn_idx );
    }
    BT_MSG_API( "BT RC EV TX: PingRes OK %x AID %x Sidx %x",
                ev_rc_pr.ev_msg.ev_rc_ping_result.success,
                ev_rc_pr.ev_hdr.bt_app_id,
                rxi_ptr->ssn_ptr->ssn_idx );
    bt_ec_send_event( &ev_rc_pr );
  }
  else
  {
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Unexp MCC Test Resp ExpM %x Sidx %x",
            rxi_ptr->ssn_ptr->mcc.expected_rx_mask,
            rxi_ptr->ssn_ptr->ssn_idx, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_fcon_command

DESCRIPTION
  Processes a Flow Control On (FCon) command message received on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_fcon_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  BT_MSG_SIG( "BT RC: RX FCon Cmd CID %x",
              rxi_ptr->ssn_ptr->cid, 0, 0 );

  if ( rxi_ptr->mcch_length == 0 )
  {
    if ( rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled == FALSE )
    {
      if ( rxi_ptr->ssn_ptr->mcc.far_v24_signals &
             RCMCCM_MSC_V24_FC_FLOW_OFF_B )
      {
        rxi_ptr->ssn_ptr->mcc.far_v24_signals &=
          ~RCMCCM_MSC_V24_FC_FLOW_OFF_B;
  
        bt_rc_restart_tx_flow();
      
#ifdef BT_FLOW_DEBUG
        BT_MSG_HIGH( "BT FLOW: RC SSN TX flow ON CID %x",
                     rxi_ptr->ssn_ptr->cid, 0, 0 );
#endif
      }
      bt_rc_tx_fc_response( rxi_ptr->ssn_ptr );
    }
    else
    {
      /*  Received FCon command message with CBFC enabled.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Unexp FCOn Cmd CBFC %x CID %x",
              rxi_ptr->dlc_ptr->config.cbfc_enabled,
              rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  Non-zero MCC length for FCon command message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: FCon Cmd Bad Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_fcon_response

DESCRIPTION
  Processes a Flow Control On (FCon) command response message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_fcon_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  BT_MSG_SIG( "BT RC: RX FCon Resp CID %x",
              rxi_ptr->ssn_ptr->cid, 0, 0 );

  if ( rxi_ptr->mcch_length == 0 )
  {
    if ( (rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled == FALSE) &&
         (rxi_ptr->ssn_ptr->mcc.expected_rx_mask &
            BT_ERXM_FCON_RESP_B) )
    {
      rxi_ptr->ssn_ptr->mcc.expected_rx_mask &= ~BT_ERXM_FCON_RESP_B;
    }
    else
    {
      /*  Unexpected FCon command response message.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Unexp FCon Resp RXM %x CBFC %x CID %x",
              rxi_ptr->ssn_ptr->mcc.expected_rx_mask,
              rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled,
              rxi_ptr->ssn_ptr->cid );
    }
  }
  else
  {
    /*  Non-zero MCC length for FCon command response message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad FCon Resp Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_fcoff_command

DESCRIPTION
  Processes a Flow Control Off (FCoff) command message received on the
  MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_fcoff_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  BT_MSG_SIG( "BT RC: RX FCoff Cmd CID %x",
              rxi_ptr->ssn_ptr->cid, 0, 0 );

  if ( rxi_ptr->mcch_length == 0 )
  {
    if ( rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled == FALSE )
    {
      if ( !(rxi_ptr->ssn_ptr->mcc.far_v24_signals &
               RCMCCM_MSC_V24_FC_FLOW_OFF_B) )
      {
        rxi_ptr->ssn_ptr->mcc.far_v24_signals |=
          RCMCCM_MSC_V24_FC_FLOW_OFF_B;
      
  #ifdef BT_FLOW_DEBUG
        BT_MSG_HIGH( "BT FLOW: RC SSN TX flow OFF CID %x",
                     rxi_ptr->ssn_ptr->cid, 0, 0 );
  #endif
      }
      bt_rc_tx_fc_response( rxi_ptr->ssn_ptr );
    }
    else
    {
      /*  Received FCoff command message with CBFC enabled.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Unexp FCOff Cmd CBFC %x CID %x",
              rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled,
              rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  Non-zero MCC length for FCoff command message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad FCoff Cmd Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_fcoff_response

DESCRIPTION
  Processes a Flow Control Off (FCoff) command response message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_fcoff_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  BT_MSG_SIG( "BT RC: RX FCoff Resp CID %x",
              rxi_ptr->ssn_ptr->cid, 0, 0 );

  if ( rxi_ptr->mcch_length == 0 )
  {
    if ( (rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled == FALSE) &&
         (rxi_ptr->ssn_ptr->mcc.expected_rx_mask &
            BT_ERXM_FCOFF_RESP_B) )
    {
      rxi_ptr->ssn_ptr->mcc.expected_rx_mask &= ~BT_ERXM_FCOFF_RESP_B;
    }
    else
    {
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Unexp FCoff Resp RXM %x CBFC %x CID %x",
              rxi_ptr->ssn_ptr->mcc.expected_rx_mask,
              rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled,
              rxi_ptr->ssn_ptr->cid );
    }
  }
  else
  {
    /*  Non-zero MCC length for FCoff command response message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad FCoff Resp Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_nsc_response

DESCRIPTION
  Processes a Non Supported Command (NSC) response message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_nsc_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  bt_rc_mcc_nsc_type  nsc_msg;

  BT_MSG_SIG( "BT RC: RX NSC Resp CID %x",
              rxi_ptr->ssn_ptr->cid, 0, 0 );

  if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_nsc_type ) )
  {
    if ( (dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &nsc_msg,
                     sizeof( bt_rc_mcc_nsc_type ))) ==
          sizeof( bt_rc_mcc_nsc_type ) )
    {
      // BT_TBD - Act on received NSC response here...
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: Unexp NSC Resp CID %x",
              rxi_ptr->ssn_ptr->cid, 0, 0 );
    }
    else
    {
      /*  Not enough data in DSM buffer for NSC response message.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: NSC Resp short Len %x CID %x",
              rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC length not correct for NSC response message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad NSC Resp Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_msc_command

DESCRIPTION
  Processes a Modem Status Command (MSC) message received on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_msc_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8                dlci;
  uint8                i, j;
  bt_rc_dlc_type*      msc_dlc_ptr = NULL;
  bt_rc_mcc_msce_type  msce_msg;
  uint8                ms_changed_mask;
  boolean              break_present;
  uint8                break_length;
  uint32               dsm_pkt_len;

  if ( (rxi_ptr->mcch_length == sizeof( bt_rc_mcc_msc_type )) ||
       (rxi_ptr->mcch_length >= sizeof( bt_rc_mcc_msce_type )) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &msce_msg,
                sizeof( bt_rc_mcc_msc_type ) );
    
    if ( RCMCCM_MDLCI_IS_OK( msce_msg.msc_body.dlci ) )
    {
      dlci = RCMCCM_MDLCI_2_DLCI( msce_msg.msc_body.dlci );

      if ( (msc_dlc_ptr =
              bt_rc_find_dlc_entry_dlci( rxi_ptr->ssn_ptr,
                                         dlci )) != NULL )
      {
        if ( rxi_ptr->mcch_length >= sizeof( bt_rc_mcc_msce_type ) )
        {
          dsm_pullup( &rxi_ptr->dsm_ptr,
                      (void *) &msce_msg.break_signals,
                      BT_RC_MSCE_EXTRA_BYTES );

          /*  Throw away extra bytes if any.  This could happen  */
          /*  with later versions of the RFCOMM specification.   */

          i = rxi_ptr->mcch_length - sizeof( bt_rc_mcc_msce_type );
          dsm_pkt_len = dsm_length_packet( rxi_ptr->dsm_ptr );

          if ( i == dsm_pkt_len )
          {
            for ( j = 0; j < i; j++ )
            {
              (void) dsm_pull8( &rxi_ptr->dsm_ptr );
            }
          }
          else
          {
            /*  MSC command length is inconsistent.  */
            rxi_ptr->error = TRUE;
            BT_ERR( "BT RC: Bad MSC Cmd Hlen %x Len %x DLCI %x",
                    rxi_ptr->mcch_length,
                    dsm_pkt_len,
                    msc_dlc_ptr->dlci );
          }
        }
      }
      else
      {
        /*  DLC for MSC command message not found.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: MSC Cmd no DLCI %x CID %x",
                dlci, rxi_ptr->ssn_ptr->cid, 0 );
      }
    }
    else
    {
      /*  MSC command DLCI field is illegal.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: MSC Cmd Bad DLCI %x CID %x",
              msce_msg.msc_body.dlci, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC header length not enough for MSC command message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: MSC Cmd bad HLen %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

  if ( rxi_ptr->error == FALSE )
  {
    if ( rxi_ptr->mcch_length >= sizeof( bt_rc_mcc_msce_type ) )
    {
      BT_MSG_SIG( "BT RC: RX MSC Cmd V24 %x Brk %x PID %x",
                  msce_msg.msc_body.v24_signals,
                  msce_msg.break_signals,
                  msc_dlc_ptr->pid );
    }
    else
    {
      BT_MSG_SIG( "BT RC: RX MSC Cmd V24 %x PID %x DLCI %x",
                  msce_msg.msc_body.v24_signals,
                  msc_dlc_ptr->pid,
                  msc_dlc_ptr->dlci );
    }

    if ( msc_dlc_ptr->config.cbfc_enabled != FALSE )
    {
      /*  Ignore FC bit if CBFC is enabled.  */
      if ( msce_msg.msc_body.v24_signals &
             RCMCCM_MSC_V24_FC_FLOW_OFF_B )
      {
        BT_ERR( "BT RC: Bad FC %x CBFC %x PID %x",
                msce_msg.msc_body.v24_signals,
                rxi_ptr->dlc_ptr->config.cbfc_enabled,
                msc_dlc_ptr->pid );
      }
      msce_msg.msc_body.v24_signals &=
        ~RCMCCM_MSC_V24_FC_FLOW_OFF_B;
    }
    else
    {
      if ( (msc_dlc_ptr->far_v24_signals &
              RCMCCM_MSC_V24_FC_FLOW_OFF_B) !=
           (msce_msg.msc_body.v24_signals &
              RCMCCM_MSC_V24_FC_FLOW_OFF_B) )
      {
        if ( !(msce_msg.msc_body.v24_signals &
                 RCMCCM_MSC_V24_FC_FLOW_OFF_B) )
        {
#ifdef BT_FLOW_DEBUG
          BT_MSG_HIGH( "BT FLOW: Far RC RX flow ON PID %x",
                       msc_dlc_ptr->pid, 0, 0 );
#endif
          bt_rc_restart_tx_flow();
        }
#ifdef BT_FLOW_DEBUG
        else
        {
          BT_MSG_HIGH( "BT FLOW: Far RC RX flow OFF PID %x",
                       msc_dlc_ptr->pid, 0, 0 );
        }
#endif
      }
    }
    
    ms_changed_mask = ( (msce_msg.msc_body.v24_signals ^
                           msc_dlc_ptr->far_v24_signals) &
                             RCMCCM_MSC_V24_MS_SIGS_M );
    
    msc_dlc_ptr->far_v24_signals = msce_msg.msc_body.v24_signals;
    
    if ( (rxi_ptr->mcch_length >= sizeof( bt_rc_mcc_msce_type )) &&
         (msce_msg.break_signals & RCMCCM_MSCE_BRK_SIG_B) )
    {
      msc_dlc_ptr->far_breaks++;
      msc_dlc_ptr->far_last_break_length =
        RCMCCM_MSCE_BKS_2_LEN( msce_msg.break_signals );
      break_present = TRUE;
      break_length  = msc_dlc_ptr->far_last_break_length;
    }
    else
    {
      break_present = FALSE;
      break_length  = 0;
    }

    bt_rc_tx_msc_response( rxi_ptr, &msce_msg );
  
    msc_dlc_ptr->expected_rx_mask &= ~BT_ERXM_MSC_CMD_B;
  
    if ( msc_dlc_ptr->state == BT_RCDS_CONNECTING_DLC )
    {
      msc_dlc_ptr->state = BT_RCDS_CONNECTED;
  
      /*  Set Credit Based Flow Control for session as necessary.  */
      if ( rxi_ptr->ssn_ptr->dlc_count == 1 )
      {
        rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled =
          msc_dlc_ptr->config.cbfc_enabled;
      }
      
      BT_MSG_HIGH( "BT RC: CONN AID %x MFS %x PID %x",
                   msc_dlc_ptr->bt_app_id,
                   msc_dlc_ptr->config.max_frame_size,
                   msc_dlc_ptr->pid );
  
      bt_rc_ev_send_connected( msc_dlc_ptr->bt_app_id,
                               &rxi_ptr->ssn_ptr->bd_addr,
                               msc_dlc_ptr->pid,
                               &msc_dlc_ptr->config );
    
      /*  Provide all initial modem status settings.  */
      bt_rc_ev_send_ms_changed( msc_dlc_ptr->bt_app_id,
                                msc_dlc_ptr->pid,
                                RCMCCM_MSC_V24_MS_SIGS_M,
                                msc_dlc_ptr->far_v24_signals,
                                break_present,
                                break_length,
                                msc_dlc_ptr->far_breaks );
    }
    if ( (msc_dlc_ptr->state == BT_RCDS_CONNECTED) &&
         ((ms_changed_mask != 0) || (break_present != FALSE)) )
    {
      bt_rc_ev_send_ms_changed( msc_dlc_ptr->bt_app_id,
                                msc_dlc_ptr->pid,
                                ms_changed_mask,
                                msc_dlc_ptr->far_v24_signals,
                                break_present,
                                break_length,
                                msc_dlc_ptr->far_breaks );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_msc_response

DESCRIPTION
  Processes a Modem Status response message received on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_msc_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8                dlci;
  uint8                i, j;
  bt_rc_dlc_type*      msc_dlc_ptr = NULL;
  bt_rc_mcc_msce_type  msce_msg;
  boolean              break_present;
  uint8                break_length;
  uint32               dsm_pkt_len;

  if ( (rxi_ptr->mcch_length == sizeof( bt_rc_mcc_msc_type )) ||
       (rxi_ptr->mcch_length >= sizeof( bt_rc_mcc_msce_type )) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &msce_msg,
                sizeof( bt_rc_mcc_msc_type ) );

    if ( RCMCCM_MDLCI_IS_OK( msce_msg.msc_body.dlci ) )
    {
      dlci = RCMCCM_MDLCI_2_DLCI( msce_msg.msc_body.dlci );

      if ( (msc_dlc_ptr =
              bt_rc_find_dlc_entry_dlci( rxi_ptr->ssn_ptr,
                                         dlci )) != NULL )
      {
        if ( msc_dlc_ptr->expected_rx_mask & BT_ERXM_MSC_RESP_B )
        {
          if ( msce_msg.msc_body.v24_signals ==
                 msc_dlc_ptr->last_msc_tx_v24_signals )
          {
            if ( rxi_ptr->mcch_length >= sizeof( bt_rc_mcc_msce_type ) )
            {
              dsm_pullup( &rxi_ptr->dsm_ptr,
                          (void *)(&msce_msg.break_signals),
                          BT_RC_MSCE_EXTRA_BYTES );

              /*  Throw away extra bytes if any.  This could happen  */
              /*  with later versions of the RFCOMM specification.   */

              i = rxi_ptr->mcch_length - sizeof( bt_rc_mcc_msce_type );
              dsm_pkt_len = dsm_length_packet( rxi_ptr->dsm_ptr );

              if ( i == dsm_pkt_len )
              {
                for ( j = 0; j < i; j++ )
                {
                  (void) dsm_pull8( &rxi_ptr->dsm_ptr );
                }
              }
              else
              {
                /*  MSC response length is inconsistent.  */
                rxi_ptr->error = TRUE;
                BT_ERR( "BT RC: Bad MSC Resp Hlen %x %x PID %x",
                        rxi_ptr->mcch_length,
                        dsm_pkt_len,
                        msc_dlc_ptr->pid );
              }
            }
            else
            {
              msce_msg.break_signals = 0x0;
            }
            if ( rxi_ptr->error == FALSE )
            {
              if ( msce_msg.break_signals !=
                     msc_dlc_ptr->last_msc_tx_break_signals )
              {
                rxi_ptr->error = TRUE;
                BT_ERR( "BT RC: Bad MSC Resp Brks %x %x %x",
                        rxi_ptr->mcch_length, msce_msg.break_signals,
                        msc_dlc_ptr->last_msc_tx_break_signals );
              }
            }
          }
          else
          {
            rxi_ptr->error = TRUE;
            BT_ERR( "BT RC: Bad MSC Resp V24 %02x %02x PID %x",
                    msce_msg.msc_body.v24_signals,
                    msc_dlc_ptr->last_msc_tx_v24_signals,
                    msc_dlc_ptr->pid );
          }
        }
        else
        {
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Unexp MSC Resp %x PID %x",
                  msc_dlc_ptr->expected_rx_mask,
                  msc_dlc_ptr->pid, 0 );
        }
      }
      else
      {
        /*  DLC for MSC response message not found.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: MSC Resp no DLCI %x CID %x",
                dlci, rxi_ptr->ssn_ptr->cid, 0 );
      }
    }
    else
    {
      /*  MSC response DLCI field is illegal.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: MSC Resp Bad DLCI %x CID %x",
              msce_msg.msc_body.dlci, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC length not enough for MSC response message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: MSC Resp bad HLen %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }
  
  if ( rxi_ptr->error == FALSE )
  {
    if ( rxi_ptr->mcch_length >= sizeof( bt_rc_mcc_msce_type ) )
    {
      BT_MSG_SIG( "BT RC: RX MSC Resp V24 %x Brk %x PID %x",
                  msce_msg.msc_body.v24_signals,
                  msce_msg.break_signals,
                  msc_dlc_ptr->pid );
    }
    else
    {
      BT_MSG_SIG( "BT RC: RX MSC Resp V24 %x PID %x DLCI %x",
                  msce_msg.msc_body.v24_signals,
                  msc_dlc_ptr->pid,
                  msc_dlc_ptr->dlci );
    }

    msc_dlc_ptr->expected_rx_mask &= ~BT_ERXM_MSC_RESP_B;

    if ( (msc_dlc_ptr->state == BT_RCDS_CONNECTING_DLC) &&
         (!(msc_dlc_ptr->expected_rx_mask & BT_ERXM_MSC_CMD_B)) )
    {
      msc_dlc_ptr->state = BT_RCDS_CONNECTED;
  
      /*  Set Credit Based Flow Control for session as necessary.  */
      if ( rxi_ptr->ssn_ptr->dlc_count == 1 )
      {
        rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled =
          msc_dlc_ptr->config.cbfc_enabled;
      }

      BT_MSG_HIGH( "BT RC: CONN AID %x MFS %x PID %x",
                   msc_dlc_ptr->bt_app_id,
                   msc_dlc_ptr->config.max_frame_size,
                   msc_dlc_ptr->pid );
  
      bt_rc_ev_send_connected( msc_dlc_ptr->bt_app_id,
                               &rxi_ptr->ssn_ptr->bd_addr,
                               msc_dlc_ptr->pid,
                               &msc_dlc_ptr->config );
    
      if ( msc_dlc_ptr->far_breaks != 0 )
      {
        break_present = TRUE;
        break_length  = msc_dlc_ptr->far_last_break_length;
      }
      else
      {
        break_present = FALSE;
        break_length  = 0;
      }
      
      /*  Provide all initial modem status settings.  */
      bt_rc_ev_send_ms_changed( msc_dlc_ptr->bt_app_id,
                                msc_dlc_ptr->pid,
                                RCMCCM_MSC_V24_MS_SIGS_M,
                                msc_dlc_ptr->far_v24_signals,
                                break_present,
                                break_length,
                                msc_dlc_ptr->far_breaks );
    }
    bt_rc_tx_msc_command( rxi_ptr->ssn_ptr, msc_dlc_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_pn_command

DESCRIPTION
  Processes a DLC Parameter Negotiation (PN) command message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_pn_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8                    dlci, pn_priority;
  bt_rc_dlc_type*          pn_dlc_ptr;
  bt_rc_mcc_pn_type        pn_msg;
  bt_rc_negot_table_type*  nte_ptr;
  bt_rc_reg_table_type*    reg_ptr;
  uint32                   tmp_mfs;

  if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_pn_type ) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &pn_msg,
                sizeof( bt_rc_mcc_pn_type ) );

    dlci = (pn_msg.dlci & RCMCCM_PN_DLCI_M);

    BT_MSG_SIG( "BT RC: RX PN Cmd MFS %x CL %x DLCI %x",
                 pn_msg.max_frame_size,
                 pn_msg.cl_field, dlci );
    
    pn_msg.unused1 = 0;  /*  Unused fields are ignored.     */
    pn_msg.unused2 = 0;  /*    Force to 0 within response.  */

    if ( (pn_dlc_ptr =
          bt_rc_find_dlc_entry_dlci( rxi_ptr->ssn_ptr,
                                     dlci )) == NULL )
    {
      nte_ptr = bt_rc_find_negot_table_entry( dlci,
                  &rxi_ptr->ssn_ptr->bd_addr, TRUE );

      nte_ptr->config.pn_negot_done = TRUE;

      pn_priority = pn_msg.priority & RCMCCM_PN_PRIORITY_M;

      if ( pn_priority > BT_RC_PRI_MAX )
      {
        rxi_ptr->error = TRUE;
        pn_priority = bt_rc_get_default_priority_for_dlci( dlci );
        BT_ERR( "BT RC: PN Bad Req Pri %x Resp %x DLCI %x",
                (pn_msg.priority & RCMCCM_PN_PRIORITY_M),
                pn_priority, dlci );
      }
      nte_ptr->config.priority = pn_priority;

      reg_ptr = 
        bt_rc_find_regd_server_channel( BT_DLC_DLCI_2_SC(dlci) );

      if ( (pn_msg.max_frame_size >= BT_RC_MFS_MIN) &&
           (pn_msg.max_frame_size <= BT_RC_MFS_MAX) )
      {
        if ( (reg_ptr != NULL) && (reg_ptr->rx_wm_ptr != NULL) )
        {
          tmp_mfs = bt_rc_calc_optimal_mfs(
                      rxi_ptr->ssn_ptr->mcc.config.max_frame_size,
                      reg_ptr->rx_wm_ptr );

          if ( pn_msg.max_frame_size > tmp_mfs )
          {
            BT_MSG_HIGH( "BT RC: PN Req MFS %x MTU Resp %x DLCI %x",
                         pn_msg.max_frame_size,
                         tmp_mfs,
                         dlci );
            pn_msg.max_frame_size = tmp_mfs;
          }
        }
        else if ( reg_ptr == NULL )
        {
          BT_MSG_DEBUG( "BT RC: PN Req for unreg DLCI %x",dlci,0,0);
        }
      }
      else
      {
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: PN Bad Req MFS %x Resp %x DLCI %x",
                pn_msg.max_frame_size,
                rxi_ptr->ssn_ptr->mcc.config.max_frame_size,
                dlci );
        pn_msg.max_frame_size =
          rxi_ptr->ssn_ptr->mcc.config.max_frame_size;
      }
      nte_ptr->config.max_frame_size = pn_msg.max_frame_size;

      if ( (RCMCCM_PN_CLF_2_CLVAL( pn_msg.cl_field ) ==
              RCMCCM_PN_CLVAL_CBFC_REQ_V) &&
           (rxi_ptr->ssn_ptr->mcc.config.cbfc_enabled != FALSE) )
      {
        nte_ptr->config.cbfc_enabled = TRUE;
        nte_ptr->config.cbfc_local_credits =
          RCMCCM_PN_KF_2_KVAL( pn_msg.k_field );
        nte_ptr->config.cbfc_far_credits =
          BT_RC_CBFC_CREDITS_DEFAULT;

        pn_msg.cl_field =
          RCMCCM_PN_CLVAL_2_CLF( RCMCCM_PN_CLVAL_CBFC_YES_V );
      }
      else
      {
        nte_ptr->config.cbfc_enabled       = FALSE;
        nte_ptr->config.cbfc_local_credits = 0;
        nte_ptr->config.cbfc_far_credits   = 0;
        
        pn_msg.cl_field =
          RCMCCM_PN_CLVAL_2_CLF( RCMCCM_PN_CLVAL_CBFC_NO_V );
      }
      pn_msg.k_field =
        RCMCCM_PN_KVAL_2_KF( nte_ptr->config.cbfc_far_credits );
      
      BT_MSG_SIG( "BT RC: PN Req CBFC %x LCr %x FCr %x",
                  nte_ptr->config.cbfc_enabled,
                  nte_ptr->config.cbfc_local_credits,
                  nte_ptr->config.cbfc_far_credits );

      bt_rc_tx_pn_response( rxi_ptr, &pn_msg );
    }
    else
    {
      /*  DLC for PN command already exists.  Refuse      */
      /*  negotiation by reporting our current settings.  */
      
      BT_MSG_SIG( "BT RC: PN Cmd Act MFS %x CL %x DLCI %x",
                  pn_msg.max_frame_size,
                  pn_msg.cl_field, dlci );

      pn_msg.priority = (pn_dlc_ptr->config.priority &
                           RCMCCM_PN_PRIORITY_M);

      pn_msg.max_frame_size = pn_dlc_ptr->config.max_frame_size;

      /*  Credit Based Flow Control fields must  */
      /*   be zero after DLC is established.     */
      pn_msg.cl_field = RCMCCM_PN_CLVAL_2_CLF( 0 );
      pn_msg.k_field = RCMCCM_PN_KVAL_2_KF( 0 );
      
      bt_rc_tx_pn_response( rxi_ptr, &pn_msg );
    }
  }
  else
  {
    /*  MCC length incorrect for PN command message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad PN Cmd Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }
  
}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_pn_response

DESCRIPTION
  Processes a DLC Parameter Negotiation (PN) response message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_pn_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8              dlci;
  bt_rc_dlc_type*    pn_dlc_ptr;
  bt_rc_mcc_pn_type  pn_msg;

  if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_pn_type ) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &pn_msg,
                sizeof( bt_rc_mcc_pn_type ) );

    dlci = (pn_msg.dlci & RCMCCM_PN_DLCI_M);

    BT_MSG_SIG( "BT RC: RX PN Resp MFS %x CLK %x DLCI %x",
                 pn_msg.max_frame_size,
                 (pn_msg.cl_field | pn_msg.k_field),
                 dlci );
    
    if ( (pn_dlc_ptr =
            bt_rc_find_dlc_entry_dlci( rxi_ptr->ssn_ptr,
                                       dlci )) != NULL )
    {
      if ( (pn_dlc_ptr->state == BT_RCDS_CONNECTING_DLC) &&
           (pn_dlc_ptr->expected_rx_mask & BT_ERXM_PN_RESP_B) )
      {
        pn_dlc_ptr->expected_rx_mask &= ~BT_ERXM_PN_RESP_B;
        
        if ( (pn_dlc_ptr->config.cbfc_enabled != FALSE) &&
             (pn_msg.cl_field ==
                RCMCCM_PN_CLVAL_2_CLF( RCMCCM_PN_CLVAL_CBFC_YES_V )) )
        {
          /*  Credit based flow control confirmed to be used.  */
          pn_dlc_ptr->config.cbfc_local_credits =
            RCMCCM_PN_KF_2_KVAL( pn_msg.k_field );
        }
        else
        {
          /*  Credit based flow control not allowed.  */
          pn_dlc_ptr->config.cbfc_enabled       = FALSE;
          pn_dlc_ptr->config.cbfc_local_credits = 0;
          pn_dlc_ptr->config.cbfc_far_credits   = 0;
        }
        BT_MSG_SIG( "BT RC: PN Resp CBFC %x LCr %x FCr %x",
                    pn_dlc_ptr->config.cbfc_enabled,
                    pn_dlc_ptr->config.cbfc_local_credits,
                    pn_dlc_ptr->config.cbfc_far_credits );
        
        if ( pn_dlc_ptr->config.priority !=
             (pn_msg.priority & RCMCCM_PN_PRIORITY_M) )
        {
          BT_MSG_SIG( "BT RC: PN Resp Pri %x was %x PID %x",
                      (pn_msg.priority & RCMCCM_PN_PRIORITY_M ),
                      pn_dlc_ptr->config.priority,
                      pn_dlc_ptr->pid );
          
          pn_dlc_ptr->config.priority = (pn_msg.priority &
                                           RCMCCM_PN_PRIORITY_M);
        }
        
        if ( pn_dlc_ptr->config.max_frame_size !=
               pn_msg.max_frame_size )
        {
          if ( pn_dlc_ptr->config.max_frame_size >
                 pn_msg.max_frame_size )
          {
            BT_MSG_SIG( "BT RC: PN Resp MFS %x was %x PID %x",
                        pn_msg.max_frame_size,
                        pn_dlc_ptr->config.max_frame_size,
                        pn_dlc_ptr->pid );

            pn_dlc_ptr->config.max_frame_size =
              pn_msg.max_frame_size;
          }
          else
          {
            BT_ERR( "BT RC: PN Resp Bad MFS %x cur %x PID %x",
                        pn_msg.max_frame_size,
                        pn_dlc_ptr->config.max_frame_size,
                        pn_dlc_ptr->pid );
          }
        }
        pn_dlc_ptr->config.pn_negot_done = TRUE;
        bt_rc_bring_up_local_client_dlc( rxi_ptr->ssn_ptr,
                                         pn_dlc_ptr );
      }
      else
      {
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Unexp PN Resp %x St %x PID %x",
                pn_dlc_ptr->expected_rx_mask,
                pn_dlc_ptr->state, pn_dlc_ptr->pid );
      }
    }
    else
    {
      /*  No DLC entry for DLCI of PN response.  */

      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: PN Resp No DLCI %x CID %x",
              dlci, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC length not correct for PN response message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad PN Resp Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_rpn_command

DESCRIPTION
  Processes a Remote Port Negotiation (RPN) command message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_rpn_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8                    dlci;
  boolean                  sp_cfg_changed;
  bt_rc_mcc_rpne_type      rpne_msg;
  bt_rc_dlc_type*          rpn_dlc_ptr;
  bt_rc_config_type*       cfg_ptr;
  bt_rc_negot_table_type*  nte_ptr;
  bt_ev_msg_type           ev_rc_recfg;

  if ( (rxi_ptr->mcch_length == sizeof( bt_rc_mcc_rpn_type )) ||
       (rxi_ptr->mcch_length == sizeof( bt_rc_mcc_rpne_type )) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &rpne_msg,
                sizeof( bt_rc_mcc_rpn_type ) );
      
    if ( RCMCCM_MDLCI_IS_OK( rpne_msg.rpn_body.dlci ) )
    {
      dlci = RCMCCM_MDLCI_2_DLCI( rpne_msg.rpn_body.dlci );
  
      if ( (rpn_dlc_ptr =
              bt_rc_find_dlc_entry_dlci( rxi_ptr->ssn_ptr,
                                         dlci )) != NULL )
      {
        cfg_ptr = &rpn_dlc_ptr->config;
      }
      else
      {
        nte_ptr = bt_rc_find_negot_table_entry( dlci,
                    &rxi_ptr->ssn_ptr->bd_addr, TRUE );
  
        cfg_ptr = &nte_ptr->config;
      }
  
      if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_rpn_type ) )
      {
        BT_MSG_SIG( "BT RC: RX RPN Req DLCI %x CID %x",
                    dlci, rxi_ptr->ssn_ptr->cid, 0 );
  
        rpne_msg.baudrate   = cfg_ptr->baudrate;
        rpne_msg.format     = cfg_ptr->format;
        rpne_msg.flow_ctrl  = cfg_ptr->flow_ctrl;
        rpne_msg.xon_char   = cfg_ptr->xon_char;
        rpne_msg.xoff_char  = cfg_ptr->xoff_char;
        rpne_msg.param_mask = RCMCCM_RPN_PM_DEFAULT_V;
  
        bt_rc_tx_rpn_response( rxi_ptr, &rpne_msg );
      }
      else
      {
        dsm_pullup( &rxi_ptr->dsm_ptr,
                    (void *)(&rpne_msg.baudrate),
                    BT_RC_RPNE_EXTRA_BYTES );
        
        BT_MSG_SIG( "BT RC: RX RPN Cmd PM %x DLCI %x CID %x",
                    rpne_msg.param_mask, dlci,
                    rxi_ptr->ssn_ptr->cid );
        
        sp_cfg_changed = bt_rc_write_sp_config( dlci, cfg_ptr,
                                                &rpne_msg );

        bt_rc_tx_rpn_response( rxi_ptr, &rpne_msg );

        cfg_ptr->rpn_negot_done = TRUE;

        if ( (sp_cfg_changed != FALSE) && (rpn_dlc_ptr != NULL) &&
             (rpn_dlc_ptr->state == BT_RCDS_CONNECTED) )
        {
          ev_rc_recfg.ev_hdr.ev_type = BT_EV_RC_RECONFIGURED;
          ev_rc_recfg.ev_hdr.bt_app_id = rpn_dlc_ptr->bt_app_id;
          ev_rc_recfg.ev_msg.ev_rc_reconfig.pid = rpn_dlc_ptr->pid;
          
          bt_rc_ev_copy_config(
            &ev_rc_recfg.ev_msg.ev_rc_reconfig.config,
            &rpn_dlc_ptr->config );

          BT_MSG_API( "BT RC EV TX: Recfg MFS %x BR %x PID %x",
            ev_rc_recfg.ev_msg.ev_rc_reconfig.config.max_frame_size,
            ev_rc_recfg.ev_msg.ev_rc_reconfig.config.baudrate,
            ev_rc_recfg.ev_msg.ev_rc_reconfig.pid );
          bt_ec_send_event( &ev_rc_recfg );
        }
      }
    }
    else
    {
      /*  RPN command DLCI field is illegal.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: RPN Cmd Bad DLCI %x CID %x",
              rpne_msg.rpn_body.dlci, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC length not correct for RPN command message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad RPN Cmd Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_rpn_response

DESCRIPTION
  Processes a Remote Port Negotiation (RPN) response message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_rpn_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8                dlci;
  bt_rc_mcc_rpne_type  rpne_msg;
  bt_rc_dlc_type*      rpn_dlc_ptr;
  bt_ev_msg_type       ev_rc_recfg;

  if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_rpne_type ) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &rpne_msg,
                sizeof( bt_rc_mcc_rpne_type ) );
    
    if ( RCMCCM_MDLCI_IS_OK( rpne_msg.rpn_body.dlci ) )
    {
      dlci = RCMCCM_MDLCI_2_DLCI( rpne_msg.rpn_body.dlci );
  
      BT_MSG_SIG( "BT RC: RX RPN Resp DLCI %x CID %x",
                  dlci, rxi_ptr->ssn_ptr->cid, 0 );
      
      if ( (rpn_dlc_ptr = bt_rc_find_dlc_entry_dlci(
                            rxi_ptr->ssn_ptr, dlci )) != NULL )
      {
        if ( (rpn_dlc_ptr->expected_rx_mask & BT_ERXM_RPN_RESP_B) &&
             ((rpn_dlc_ptr->state == BT_RCDS_CONNECTING_DLC) ||
              (rpn_dlc_ptr->state == BT_RCDS_CONNECTED)) )
        {
          rpn_dlc_ptr->expected_rx_mask &= ~BT_ERXM_RPN_RESP_B;

          bt_rc_write_sp_config( dlci, &rpn_dlc_ptr->config, &rpne_msg );
  
          rpn_dlc_ptr->config.rpn_negot_done = TRUE;

          if ( rpn_dlc_ptr->state == BT_RCDS_CONNECTING_DLC )
          {
            bt_rc_bring_up_local_client_dlc( rxi_ptr->ssn_ptr,
                                             rpn_dlc_ptr );
          }
          else
          {
            ev_rc_recfg.ev_hdr.ev_type = BT_EV_RC_RECONFIGURED;
            ev_rc_recfg.ev_hdr.bt_app_id = rpn_dlc_ptr->bt_app_id;
            ev_rc_recfg.ev_msg.ev_rc_reconfig.pid = rpn_dlc_ptr->pid;
            
            bt_rc_ev_copy_config(
              &ev_rc_recfg.ev_msg.ev_rc_reconfig.config,
              &rpn_dlc_ptr->config );

            BT_MSG_API( "BT RC EV TX: Recfg MFS %x BR %x PID %x",
              ev_rc_recfg.ev_msg.ev_rc_reconfig.config.max_frame_size,
              ev_rc_recfg.ev_msg.ev_rc_reconfig.config.baudrate,
              ev_rc_recfg.ev_msg.ev_rc_reconfig.pid );
            bt_ec_send_event( &ev_rc_recfg );
          }
        }
        else
        {
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Unexp RPN Resp %x St %x PID %x",
                  rpn_dlc_ptr->expected_rx_mask,
                  rpn_dlc_ptr->state, rpn_dlc_ptr->pid );
        }
      }
      else
      {
        /*  No DLC entry for DLCI of RPN response.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: RPN Resp No DLCI %x CID %x",
                dlci, rxi_ptr->ssn_ptr->cid, 0 );
      }
    }
    else
    {
      /*  RPN response DLCI field is illegal.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: RPN Resp Bad DLCI %x CID %x",
              rpne_msg.rpn_body.dlci, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC length not correct for RPN response message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad RPN Resp Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_rls_command

DESCRIPTION
  Processes a Remote Line Status (RLS) command message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_rls_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8               dlci;
  bt_rc_mcc_rls_type  rls_msg;
  bt_rc_dlc_type*     rls_dlc_ptr = NULL;
  bt_ev_msg_type      ev_lerr;

  if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_rls_type ) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &rls_msg,
                sizeof( bt_rc_mcc_rls_type ) );
    
    if ( RCMCCM_MDLCI_IS_OK( rls_msg.dlci ) )
    {
      dlci = RCMCCM_MDLCI_2_DLCI( rls_msg.dlci );
  
      BT_MSG_SIG( "BT RC: RX RLS Cmd V1 %x DLCI %x CID %x",
                  rls_msg.value1, dlci, rxi_ptr->ssn_ptr->cid );
      
      if ( (rls_dlc_ptr =
              bt_rc_find_dlc_entry_dlci( rxi_ptr->ssn_ptr,
                                         dlci )) != NULL )
      {
        if ( rls_msg.value1 & RCMCCM_RLS_V1_LS_ERR_B )
        {
          ev_lerr.ev_msg.ev_rc_line_error.cur_line_error =
            BT_RCLE_NONE;

          switch ( RCMCCM_RLS_V1_2_LEV( rls_msg.value1 ) )
          {
            case RCMCCM_RLS_V1_LSET_OVERRUN_V:
              {
                rls_dlc_ptr->far_overruns++;
                ev_lerr.ev_msg.ev_rc_line_error.cur_line_error =
                  BT_RCLE_OVERRUN_ERROR;
                BT_MSG_SIG( "BT RC: RLS Overruns %x PID %x DLCI %x",
                            rls_dlc_ptr->far_overruns,
                            rls_dlc_ptr->pid, rls_dlc_ptr->dlci );
                break;
              }
            
            case RCMCCM_RLS_V1_LSET_PARITY_V:
              {
                rls_dlc_ptr->far_parity_errors++;
                ev_lerr.ev_msg.ev_rc_line_error.cur_line_error =
                  BT_RCLE_PARITY_ERROR;
                BT_MSG_SIG( "BT RC: RLS Prty errs %x PID %x DLCI %x",
                            rls_dlc_ptr->far_parity_errors,
                            rls_dlc_ptr->pid, rls_dlc_ptr->dlci );
                break;
              }
  
            case RCMCCM_RLS_V1_LSET_FRAMING_V:
              {
                rls_dlc_ptr->far_framing_errors++;
                ev_lerr.ev_msg.ev_rc_line_error.cur_line_error =
                  BT_RCLE_FRAMING_ERROR;
                BT_MSG_SIG( "BT RC: RLS Frmg errs %x PID %x DLCI %x",
                            rls_dlc_ptr->far_framing_errors,
                            rls_dlc_ptr->pid, rls_dlc_ptr->dlci );
                break;
              }
  
            default:
              {
                rxi_ptr->error = TRUE;
                BT_ERR( "BT RC: Bad RLS Cmd V1 %x PID %x DLCI %x",
                        rls_msg.value1, rls_dlc_ptr->pid,
                        rls_dlc_ptr->dlci );
                break;
              }
          }
          if ( ev_lerr.ev_msg.ev_rc_line_error.cur_line_error !=
                BT_RCLE_NONE )
          {
            ev_lerr.ev_hdr.ev_type = BT_EV_RC_LINE_ERROR;
            ev_lerr.ev_hdr.bt_app_id = rls_dlc_ptr->bt_app_id;
            ev_lerr.ev_msg.ev_rc_line_error.pid = rls_dlc_ptr->pid;
            ev_lerr.ev_msg.ev_rc_line_error.overruns =
              rls_dlc_ptr->far_overruns;
            ev_lerr.ev_msg.ev_rc_line_error.parity_errors =
              rls_dlc_ptr->far_parity_errors;
            ev_lerr.ev_msg.ev_rc_line_error.framing_errors =
              rls_dlc_ptr->far_framing_errors;

            BT_MSG_API( "BT RC EV TX: Line Err %x AID %x PID %x",
                        ev_lerr.ev_msg.ev_rc_line_error.cur_line_error,
                        ev_lerr.ev_hdr.bt_app_id, rls_dlc_ptr->pid );
            bt_ec_send_event( &ev_lerr );
          }
        }
        else
        {
          BT_ERR( "BT RC: RLS Cmd No Err V1 %x PID %x DLCI %x",
                  rls_msg.value1, rls_dlc_ptr->pid,
                  rls_dlc_ptr->dlci );
        }
        bt_rc_tx_rls_response( rxi_ptr, &rls_msg );
      }
      else
      {
        /*  DLC for RLS command message not found.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: RLS Cmd no DLCI %x CID %x",
                dlci, rxi_ptr->ssn_ptr->cid, 0 );
      }
    }
    else
    {
      /*  RLS command DLCI field is illegal.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: RLS Cmd Bad DLCI %x CID %x",
              rls_msg.dlci, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC length not correct for RLS command message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad RLS Cmd Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_rls_response

DESCRIPTION
  Processes a Remote Line Status (RLS) response message received
  on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_rls_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8               dlci;
  bt_rc_mcc_rls_type  rls_msg;
  bt_rc_dlc_type*     rls_dlc_ptr = NULL;

  if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_rls_type ) )
  {
    dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &rls_msg,
                sizeof( bt_rc_mcc_rls_type ) );
      
    if ( RCMCCM_MDLCI_IS_OK( rls_msg.dlci ) )
    {
      dlci = RCMCCM_MDLCI_2_DLCI( rls_msg.dlci );
  
      BT_MSG_SIG( "BT RC: RX RLS Resp DLCI %x CID %x",
                  dlci, rxi_ptr->ssn_ptr->cid, 0 );
      
      if ( (rls_dlc_ptr =
              bt_rc_find_dlc_entry_dlci( rxi_ptr->ssn_ptr,
                                         dlci )) != NULL )
      {
        if ( rls_dlc_ptr->expected_rx_mask & BT_ERXM_RLS_RESP_B )
        {
          rls_dlc_ptr->expected_rx_mask &= ~BT_ERXM_RLS_RESP_B;
      
          if ( rls_msg.value1 == rls_dlc_ptr->last_rls_tx_value1 )
          {
            rls_dlc_ptr->last_rls_tx_value1 = 0x0;
            bt_rc_tx_rls_command( rxi_ptr->ssn_ptr,
                                  rls_dlc_ptr );
          }
          else
          {
            rxi_ptr->error = TRUE;
            BT_ERR( "BT RC: RLS Resp Bad V1 %x Exp %x PID %x",
                    rls_msg.value1, rls_dlc_ptr->last_rls_tx_value1,
                    rls_dlc_ptr->pid );
          }
        }
        else
        {
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Unexp RLS Resp %x PID %x DLCI %x",
                  rls_dlc_ptr->expected_rx_mask,
                  rls_dlc_ptr->pid, rls_dlc_ptr->dlci );
        }
      }
      else
      {
        /*  DLC for RLS response message not found.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: RLS Resp no DLCI %x CID %x",
                dlci, rxi_ptr->ssn_ptr->cid, 0 );
      }
    }
    else
    {
      /*  RLS response DLCI field is illegal.  */
      rxi_ptr->error = TRUE;
      BT_ERR( "BT RC: RLS Resp Bad DLCI %x CID %x",
              rls_msg.dlci, rxi_ptr->ssn_ptr->cid, 0 );
    }
  }
  else
  {
    /*  MCC length not correct for RLS response message.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Bad RLS Resp Len %x CID %x",
            rxi_ptr->mcch_length, rxi_ptr->ssn_ptr->cid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_process_command

DESCRIPTION
  Processes a command message received on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_process_command
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  switch ( rxi_ptr->mcch_type )
  {
    case RCMCCH_TYPE_MT_TEST_V:
      {
        bt_rc_rx_mcc_test_command( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_FCON_V:
      {
        bt_rc_rx_mcc_fcon_command( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_FCOFF_V:
      {
        bt_rc_rx_mcc_fcoff_command( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_MSC_V:
      {
        bt_rc_rx_mcc_msc_command( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_PN_V:
      {
        bt_rc_rx_mcc_pn_command( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_RPN_V:
      {
        bt_rc_rx_mcc_rpn_command( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_RLS_V:
      {
        bt_rc_rx_mcc_rls_command( rxi_ptr );
        break;
      }
    default:
      {
        /*  Unrecognized MCC command message received.  */
        
        bt_rc_tx_nsc_response( rxi_ptr->ssn_ptr,
                               rxi_ptr->mcch_type );

        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad MCC Cmd Type %x CID %x",
                rxi_ptr->mcch_type, rxi_ptr->ssn_ptr->cid, 0 );
        break;
      }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_mcc_process_mcc_response

DESCRIPTION
  Processes a response message received on the MCC.

===========================================================================*/
LOCAL void bt_rc_rx_mcc_process_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  switch ( rxi_ptr->mcch_type )
  {
    case RCMCCH_TYPE_MT_TEST_V:
      {
        bt_rc_rx_mcc_test_response( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_FCON_V:
      {
        bt_rc_rx_mcc_fcon_response( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_FCOFF_V:
      {
        bt_rc_rx_mcc_fcoff_response( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_NSC_V:
      {
        bt_rc_rx_mcc_nsc_response( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_MSC_V:
      {
        bt_rc_rx_mcc_msc_response( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_PN_V:
      {
        bt_rc_rx_mcc_pn_response( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_RPN_V:
      {
        bt_rc_rx_mcc_rpn_response( rxi_ptr );
        break;
      }

    case RCMCCH_TYPE_MT_RLS_V:
      {
        bt_rc_rx_mcc_rls_response( rxi_ptr );
        break;
      }
    default:
      {
        /*  Unrecognized MCC response message received.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad MCC Resp Type %x CID %x",
                rxi_ptr->mcch_type, rxi_ptr->ssn_ptr->cid, 0 );
        break;
      }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_process_uih

DESCRIPTION
  Processes a received UIH frame.

===========================================================================*/
LOCAL void bt_rc_rx_process_uih
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  word                     pkt_length;
  bt_rc_cbfc_credits_type  cbfc_credits;
  dsm_item_type*           cur_dsm_ptr;
  dsm_item_type*           next_dsm_ptr;

  if ( rxi_ptr->fh_dlci != RC_DLCI_MCC_V )
  {
    if ( (rxi_ptr->dlc_ptr->config.cbfc_enabled != FALSE) &&
         (rxi_ptr->fh_poll_final != FALSE) )
    {
      /*  Accumulate local transmit credits  */
      /*  for Credit Based Flow Control.     */
      dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &cbfc_credits,
                  sizeof( bt_rc_cbfc_credits_type ) );

      rxi_ptr->dlc_ptr->config.cbfc_local_credits += cbfc_credits;

      if ( rxi_ptr->dlc_ptr->config.cbfc_local_credits >=
             BT_RC_CBFC_CREDITS_MAX_LOC )
      {
        BT_ERR( "BT CBFC: LTot MAX %x Cur %x PID %x",
                rxi_ptr->dlc_ptr->config.cbfc_local_credits,
                cbfc_credits, rxi_ptr->dlc_ptr->pid );
      }

#ifdef BT_FLOW_DEBUG
      BT_MSG_HIGH( "BT RC: CBFC RX LCr Tot/Cur %04x Len %x PID %x",
                   ((rxi_ptr->dlc_ptr->config.
                       cbfc_local_credits << 8) | cbfc_credits),
                   rxi_ptr->fh_frame_length, rxi_ptr->dlc_ptr->pid );
#endif

      if ( (rxi_ptr->dlc_ptr->config.cbfc_local_credits ==
              cbfc_credits ) && (cbfc_credits > 0) )
      {
        bt_rc_restart_tx_flow();
#ifdef BT_FLOW_DEBUG
        BT_MSG_HIGH( "BT RC: CBFC Local TX ON PID %x",
                     rxi_ptr->dlc_ptr->pid, 0, 0 );
#endif
      }
      
    }
    
    if ( rxi_ptr->dsm_ptr != NULL )
    {
#ifdef BT_SWDEV_QXDM_LOG_MSG
      bt_rc_send_payload_rx_log( rxi_ptr->dsm_ptr,
                                 rxi_ptr->fh_dlci,
                                 rxi_ptr->ssn_ptr->cid );
#endif
      pkt_length = dsm_length_packet( rxi_ptr->dsm_ptr );

      if ( (pkt_length > 0) &&
           (rxi_ptr->dlc_ptr->config.cbfc_enabled != FALSE) )
      {
        /*  Update far transmit credits     */
        /*  for Credit Based Flow Control.  */
        if ( rxi_ptr->dlc_ptr->config.cbfc_far_credits > 0 )
        {
          rxi_ptr->dlc_ptr->config.cbfc_far_credits--;
          
#ifdef BT_FLOW_DEBUG
          if ( (rxi_ptr->dlc_ptr->rx_wm_ptr != NULL) &&
               (rxi_ptr->dlc_ptr->config.cbfc_far_credits == 0) )
          {
            BT_MSG_HIGH( "BT RC: CBFC Far TX OFF PID %x",
                         rxi_ptr->dlc_ptr->pid, 0, 0 );
          }
#endif

          if ( bt_rc_far_tx_credits_needed( rxi_ptr->ssn_ptr,
                                            rxi_ptr->dlc_ptr ) != FALSE )
          {
            /*  Force a UIH frame transmit if none  */
            /*  pending to make sure we give some   */
            /*  transmit credits to the far side.   */
            bt_rc_restart_tx_flow();
#ifdef BT_FLOW_DEBUG
            BT_MSG_HIGH( "BT RC: CBFC ReTX FCr %x PID %x",
                         rxi_ptr->dlc_ptr->config.cbfc_far_credits,
                         rxi_ptr->dlc_ptr->pid, 0 );
#endif
          }
        }
        else
        {
          BT_ERR( "BT CBFC: FTot MIN %x PID %x",
                  rxi_ptr->dlc_ptr->config.cbfc_far_credits,
                  rxi_ptr->dlc_ptr->pid, 0 );
        }
      }

      if ( pkt_length > 0 )
      {
        BT_MSG_PYLD( "BT RC RXD: Len %x DLCI %x d1 %x",
                     pkt_length, rxi_ptr->dlc_ptr->dlci,
                     (uint8)(*(rxi_ptr->dsm_ptr->data_ptr)) );

        BT_MSG_API_PYLD( "BT RC EV TX: RXD Len %x AID %x DLCI %x",
                         pkt_length, rxi_ptr->dlc_ptr->bt_app_id,
                         rxi_ptr->dlc_ptr->dlci );
        
        rxi_ptr->dlc_ptr->rx_data_bytes += pkt_length;
        
        BT_SET_PKT_TRACER( rxi_ptr->dsm_ptr, DSM_BT_RX_RC );
        
        bt_sio_process_rc_rx_data( rxi_ptr->dlc_ptr->pid,
                                   &rxi_ptr->dsm_ptr );

        if ( rxi_ptr->dsm_ptr != NULL )
        {
          /*  Enqueue received data on RX watermark.  */
          if ( rxi_ptr->dlc_ptr->rx_wm_ptr != NULL )
          {
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
            {
              cur_dsm_ptr = rxi_ptr->dsm_ptr;
              while ( cur_dsm_ptr != NULL )
              {
                next_dsm_ptr = cur_dsm_ptr->pkt_ptr;
                cur_dsm_ptr->pkt_ptr = NULL;
                dsm_enqueue( rxi_ptr->dlc_ptr->rx_wm_ptr,
                             &cur_dsm_ptr );
                cur_dsm_ptr = next_dsm_ptr;
              }
              rxi_ptr->dsm_ptr = NULL;
            }
          }
          else
          {
            /*  Bad RX watermark pointer.  */
            rxi_ptr->error = TRUE;
            BT_ERR( "BT RC: Bad RX WM ptr PID %x DLCI %x",
                    rxi_ptr->dlc_ptr->pid, rxi_ptr->dlc_ptr->dlci, 0 );
          }
        }
      }
      else
      {
        if ( rxi_ptr->dlc_ptr->config.cbfc_enabled == FALSE )
        {
          /*  No data received in UIH frame.  */
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: UIH no data PID %x DLCI %x",
                  rxi_ptr->dlc_ptr->pid,
                  rxi_ptr->dlc_ptr->dlci, 0 );
        }
      }
    }
  }
  else
  {
    BT_MSG_SIG( "BT RC: RX UIH on MCC CID %x",
                rxi_ptr->ssn_ptr->cid, 0, 0 );

    /*  Process MCC message within UIH frame.  */
    
    bt_rc_rx_unpack_mcc_hdr( rxi_ptr );
    
    if ( rxi_ptr->error == FALSE )
    {
      pkt_length = dsm_length_packet( rxi_ptr->dsm_ptr );
      if ( rxi_ptr->mcch_length == pkt_length )
      {
        if ( rxi_ptr->mcch_command != FALSE )
        {
          bt_rc_rx_mcc_process_command( rxi_ptr );
        }
        else
        {
          bt_rc_rx_mcc_process_response( rxi_ptr );
        }
      }
      else
      {
        /*  Inconsistent MCC message length.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad MCC Hdr Len %x PLen %x CID %x",
                rxi_ptr->mcch_length,
                pkt_length,
                rxi_ptr->ssn_ptr->cid );
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_process_frame

DESCRIPTION
  Processes a received RFCOMM frame.

===========================================================================*/
void bt_rc_rx_process_frame
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  if ( rxi_ptr->fh_command != FALSE )
  {
    switch ( rxi_ptr->fh_frame_type )
    {
      case RCFH_CTRL_FT_SABM_V:
        {
          rxi_ptr->dlc_ptr->rx_sabm_frames++;
          bt_rc_rx_process_sabm( rxi_ptr );
          break;
        }
      case RCFH_CTRL_FT_DISC_V:
        {
          rxi_ptr->dlc_ptr->rx_disc_frames++;
          bt_rc_rx_process_disc( rxi_ptr );
          break;
        }
      case RCFH_CTRL_FT_UIH_V:
        {
          rxi_ptr->dlc_ptr->rx_uih_frames++;
          bt_rc_rx_process_uih( rxi_ptr );
          break;
        }
      default:
        {
          /*  Unrecognized RFCOMM command frame type.  */
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Bad Cmd FType %x DLCI %x CID %x",
                  rxi_ptr->fh_frame_type,
                  rxi_ptr->dlc_ptr->dlci,
                  rxi_ptr->ssn_ptr->cid );
          break;
        }
    }
  }
  else
  {
    switch ( rxi_ptr->fh_frame_type )
    {
      case RCFH_CTRL_FT_UA_V:
        {
          rxi_ptr->dlc_ptr->rx_ua_frames++;
          bt_rc_rx_process_ua( rxi_ptr );
          break;
        }
      case RCFH_CTRL_FT_DM_V:
        {
          rxi_ptr->dlc_ptr->rx_dm_frames++;
          bt_rc_rx_process_dm( rxi_ptr );
          break;
        }
      case RCFH_CTRL_FT_UIH_V:
        {
          rxi_ptr->dlc_ptr->rx_uih_frames++;
          bt_rc_rx_process_uih( rxi_ptr );
          break;
        }
      default:
        {
          /*  Unrecognized RFCOMM response frame type.  */
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Bad Resp FType %x DLCI %x CID %x",
                  rxi_ptr->fh_frame_type,
                  rxi_ptr->dlc_ptr->dlci,
                  rxi_ptr->ssn_ptr->cid );
          break;
        }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_rx_unpack_frame_hdr_and_fcs

DESCRIPTION
  Unpacks a received RFCOMM frame header and FCS into the passed
  rx_info structure.

RETURN VALUE
  Updates fh_* and fcs_* fields within rx_info

===========================================================================*/
void bt_rc_rx_unpack_frame_hdr_and_fcs
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint8                  fcs_byte_count;
  bt_rc_frame_hdre_type  rc_hdr;
  uint16                 temp_length;
  dsm_item_type*         last_dsm_ptr = NULL;
  dsm_item_type*         prev_dsm_ptr = NULL;
  bt_rc_fcs_type*        fcs_ptr;
  fast_uih_fcs_type*     fuf_ptr;
  uint16                 cbfc_extra = 0;
  uint32                 dsm_pkt_len;

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_rc_send_rx_signaling_log( rxi_ptr->dsm_ptr,
                               rxi_ptr->ssn_ptr->cid );
#endif

  if ( (dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &rc_hdr,
                   sizeof( bt_rc_frame_hdr_type ))) ==
        sizeof( bt_rc_frame_hdr_type ) )
  {
    rxi_ptr->fh_command = FALSE;
    if ( rxi_ptr->ssn_ptr->initiator != FALSE )
    {
      if ( !(rc_hdr.address & RC_CR_CMD_B) )
      {
        rxi_ptr->fh_command = TRUE;
      }
    }
    else
    {
      if ( rc_hdr.address & RC_CR_CMD_B )
      {
        rxi_ptr->fh_command = TRUE;
      }
    }
    rxi_ptr->fh_dlci = RCFH_ADDR_2_DLCI( rc_hdr.address );
    rxi_ptr->fh_direction = RCFH_ADDR_2_DIR( rc_hdr.address );
    rxi_ptr->fh_server_channel = RCFH_ADDR_2_SC( rc_hdr.address );

    rxi_ptr->fh_frame_type = (rc_hdr.control & RCFH_CTRL_FT_M);
    rxi_ptr->fh_poll_final = RCFH_CTRL_2_PF( rc_hdr.control );

    rxi_ptr->fh_frame_length =
      ((rc_hdr.length1 & RCFH_LEN_LSB_M) >> 1);

    if ( (rxi_ptr->fh_frame_type == RCFH_CTRL_FT_UIH_V) &&
         (rxi_ptr->fh_dlci != RC_DLCI_MCC_V) )
    {
      if ( (rxi_ptr->dlc_ptr = bt_rc_find_dlc_entry_dlci(
                                 rxi_ptr->ssn_ptr,
                                 rxi_ptr->fh_dlci )) == NULL )
      {
        /*  Could not find DLC entry for received data.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: No DLCI %x CID %x",
                rxi_ptr->fh_dlci, rxi_ptr->ssn_ptr->cid, 0 );
      }
    }

    if ( (rxi_ptr->error == FALSE) &&
         (!(rc_hdr.length1 & RC_EA_LAST_B)) )
    {
      if ( (dsm_pullup( &rxi_ptr->dsm_ptr, (void *) &rc_hdr.length2,
                        BT_RCFHE_EXTRA_BYTES )) ==
            BT_RCFHE_EXTRA_BYTES )
      {
        rxi_ptr->fh_frame_length |=
          ((rc_hdr.length2 & RCFH_LEN_MSB_M) << 7);
      }
      else
      {
        /*  Not enough data in received RFCOMM  */
        /*  frame for extended RFCOMM header.   */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Frame Ehdr no data FLen %x CID %x",
                rxi_ptr->fh_frame_length,
                rxi_ptr->ssn_ptr->cid, 0 );
      }
    }

    if ( rxi_ptr->error == FALSE )
    {
      /*  Unpack and verify FCS.  */

      /*  TBD  Is there a better way to do this via DSM code?  */
      if ( (rxi_ptr->dsm_ptr != NULL) &&
           (dsm_length_packet( rxi_ptr->dsm_ptr ) >=
           rxi_ptr->fh_frame_length + sizeof( bt_rc_fcs_type )) )
      {
        last_dsm_ptr = rxi_ptr->dsm_ptr;
        temp_length = rxi_ptr->fh_frame_length;

        /*  Account for Credit Based Flow Control      */
        /*  transmit credits in frame as appropriate.  */
        if ( (rxi_ptr->fh_frame_type == RCFH_CTRL_FT_UIH_V) &&
             (rxi_ptr->fh_dlci != RC_DLCI_MCC_V) &&
             (rxi_ptr->dlc_ptr->config.cbfc_enabled != FALSE) &&
             (rxi_ptr->fh_poll_final != FALSE) )
        {
          temp_length += sizeof( bt_rc_cbfc_credits_type );
        }

        while ( last_dsm_ptr->pkt_ptr != NULL )
        {
          temp_length -= last_dsm_ptr->used;
          prev_dsm_ptr = last_dsm_ptr;
          last_dsm_ptr = last_dsm_ptr->pkt_ptr;
        }

        fcs_ptr = (bt_rc_fcs_type*)(last_dsm_ptr->data_ptr);
        fcs_ptr += temp_length;
        rxi_ptr->fcs_received = *fcs_ptr;

        last_dsm_ptr->used -= sizeof( bt_rc_fcs_type );
        if ( last_dsm_ptr->used == 0 )
        {
          if ( rxi_ptr->dsm_ptr == last_dsm_ptr )
          {
            rxi_ptr->dsm_ptr = NULL;
          }

          /*  Free last packet since it only contains the FCS, also  */
          /*  be sure to unchain this buffer from the packet chain.  */
          dsm_free_packet( &last_dsm_ptr );
          if ( prev_dsm_ptr != NULL )
          {
            prev_dsm_ptr->pkt_ptr = NULL;
          }
        }
      }
      else
      {
        /*  Not enough data in received RFCOMM frame for FCS.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Frame FCS no data FLen %x CID %x",
                rxi_ptr->fh_frame_length,
                rxi_ptr->ssn_ptr->cid, 0 );
      }
    }

    if ( rxi_ptr->error == FALSE )
    {
      /*  FCS for UIH frames carrying application data are  */
      /*  checked via the "fast" UIH FCS check.  These are  */
      /*  UIH frames sent on a non-MCC DLCI.                */
      if ( (rxi_ptr->fh_frame_type == RCFH_CTRL_FT_UIH_V) &&
           (rxi_ptr->fh_dlci != RC_DLCI_MCC_V) )
      {
        if ( rxi_ptr->fh_poll_final == FALSE )
        {
          fuf_ptr = &rxi_ptr->dlc_ptr->rx_fast_uih_pf0;
        }
        else
        {
          fuf_ptr = &rxi_ptr->dlc_ptr->rx_fast_uih_pf1;
          if ( rxi_ptr->dlc_ptr->config.cbfc_enabled != FALSE )
          {
            cbfc_extra = sizeof( bt_rc_cbfc_credits_type );
          }
        }
        if ( (rc_hdr.address != fuf_ptr->hdr_addr) ||
             (rc_hdr.control != fuf_ptr->hdr_ctrl) ||
             (rxi_ptr->fcs_received != fuf_ptr->fcs) )
        {
          /*  FCS failed.  */
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Bad FFCS Rx %x Exp %x DLCI %x",
                  rxi_ptr->fcs_received,
                  fuf_ptr->fcs,
                  rxi_ptr->dlc_ptr->dlci );

          BT_ERR( "BT RC: Bad FFCS Addr Rx %x Exp %x",
                  rc_hdr.address,
                  fuf_ptr->hdr_addr, 0 );
          
          BT_ERR( "BT RC: Bad FFCS Ctrl Rx %x Exp %x",
                  rc_hdr.control,
                  fuf_ptr->hdr_ctrl, 0 );
          
          BT_VSTAT_INC( RC_RX_BAD_FCS_COUNT );
        }
      }
      else
      {
        if ( rxi_ptr->fh_frame_type == RCFH_CTRL_FT_UIH_V )
        {
          fcs_byte_count = BT_RC_FCS_BYTES_UIH;
        }
        else
        {
          fcs_byte_count = BT_RC_FCS_BYTES_NON_UIH;
        }
        rxi_ptr->fcs_calculated = bt_rc_calculate_fcs(
                                    (uint8*)(&(rc_hdr.address)),
                                    fcs_byte_count );
        if ( rxi_ptr->fcs_received != rxi_ptr->fcs_calculated )
        {
          /*  FCS failed.  */
          rxi_ptr->error = TRUE;
          BT_ERR( "BT RC: Bad Slow FCS Rx %x Exp %x CID %x",
                  rxi_ptr->fcs_received, rxi_ptr->fcs_calculated,
                  rxi_ptr->ssn_ptr->cid );

          BT_VSTAT_INC( RC_RX_BAD_FCS_COUNT );
        }
      }
    }

    if ( rxi_ptr->error == FALSE )
    {
      /*  Sanity checks.  */
      if ( !(rc_hdr.address & RC_EA_LAST_B) )
      {
        /*  EA bit not set in frame header address field - illegal!  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad EA Fhdr addr %x CID %x",
                rc_hdr.address, rxi_ptr->ssn_ptr->cid, 0 );
      }

      dsm_pkt_len = dsm_length_packet( rxi_ptr->dsm_ptr );
      if ( dsm_pkt_len != (rxi_ptr->fh_frame_length + cbfc_extra) )
      {
        /*  Byte count in DSM not as expected for frame.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad lengths FLen %x PLen %x CID %x",
                (rxi_ptr->fh_frame_length + cbfc_extra),
                dsm_pkt_len,
                rxi_ptr->ssn_ptr->cid );
      }

      if ( (rxi_ptr->fh_dlci != RC_DLCI_MCC_V) &&
           ((rxi_ptr->fh_server_channel < RC_SERV_CHAN_MIN_V) ||
            (rxi_ptr->fh_server_channel > RC_SERV_CHAN_MAX_V)) )
      {
        /*  Illegal server channel.  */
        rxi_ptr->error = TRUE;
        BT_ERR( "BT RC: Bad SC %x CID %x",
                rxi_ptr->fh_server_channel,
                rxi_ptr->ssn_ptr->cid, 0 );
      }
    }
  }
  else
  {
    /*  Not enough data in received RFCOMM frame.  */
    rxi_ptr->error = TRUE;
    BT_ERR( "BT RC: Fhdr no data CID %x",
            rxi_ptr->ssn_ptr->cid, 0, 0 );
  }

}

#endif /* FEATURE_BT */

