/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         B L U E T O O T H    E X T E R N A L    P R O F I L E S
  O P E N   I N T E R F A C E   L 2   G L U E   L A Y E R

GENERAL DESCRIPTION
  This module contains the glue code to implement the interface for
  OI's L2 layer using QCom's underlying L2 implementation.

              Copyright (c) 2004-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfl2if.c#1 $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-10-17   gs  Modified OI_L2CAP_Disconnect() to do disconnect immediate
                   (of underlying ACL link if there are no other connections).
 
*
*    #23       21 Feb 2007           PN
* Fixed critical lint errors.
*
*    #22       21 Dec  2006          DM
* Changed the OI_L2CAP_Register() interface to pass the config parameters 
* to be used for the registered PSM
*
*    #21       07 Jul  2006          DM
* For flushable packets that dont need a tx-cfm the callback is 
* not called anymore (Av-Sync optimization)
*
*    #20       26 Jul  2006          DM
* Changes for AV-Sync - many changes to OI_L2CAP_WriteMBUF
* Handling of l2cap write cfm
*
*    #19       22 Mar  2006          JTL
* More closely mimic what the upper layers are expeting: don't allow servers
* to deregister if there are open incoming connections.
*
*    #18       10 Oct  2005          JTL
* Fixing corner case of RX data before app has accepted connection and given
* cb fcn for the data. Currently only handles one L2cap packet, will still
* drop data if multiple packets arrive before the CB has been set up.
*
*    #17       02 Aug  2005          JTL
* Fixing memory leak in RX data.
*
*    #16       26 Jul  2005          JTL
* Fixing rejection of L2 connections.
*
*    #15       01 Jul  2005          JTL
* More error messages.
* Don't leak BT app IDs.
*
*    #14       29 Jun  2005          JTL
* Don't overwrite DB entry for connections which are already in use.
*
*    #13       10 May  2005          GS
* Removed bt_pf_l2if_init_db routine with flawed init logic. L2 table
* initialization is now done in bt_pf_l2if_init()
*
*    #12       02 May  2005          JTL
* For all OI L2 connections, automatically disable the idle timeout in
* the lower layer if an upper layer is informed.
*
*    #11       27 Apr  2005          JTL
* Handle having a registered server on a PSM, and connecting to a remote
* device with that PSM at the same time.
* Better debug messages.
* Even better handling of getting & clearing table entries.
*
*    #10       22 Apr  2005          JTL
* Better/more consistent clearing of table entries. Doesn't loose entries
* for missed pages any more.
* Clean up a bit of code looking up items in the tables.
*
*    #8-9      08 Mar  2005          JTL
* Better handling of disconnect events. Don't dereference null pointers.
*
*    #7        04 Mar  2005          JTL
* Add support for sending MBUFs.
* Add support for having multiple outgoing connections to the same PSM.
*
*    #6        07 Nov  2004          JTL
* Different stubs for CLT. Tell upper layer that we support it, even though
* we don't.
*
*    #5        07 Oct  2004          JTL
* Don't be too agressive about cleaning up code. In particular, leave the
* CID around in the DB until the disconnection event.
*
*    #4        10 Aug  2004          JTL
* Better cleanup code at the end of connections.
*
*    #3        06 Aug  2004          JTL
* Many fixes. Rearranged state variables to keep track of only one client
* per PSM, rather than supporting different clients for incoming/outgoing
* connections.
* Queue events so they are processed in the BTPF task, not the BT task.
* Currently working for basic L2 connections.
*
*    #2        02 Aug  2004          JTL
* Move map_event_code to another place.
*
*    #1        28 July 2004          JTL
* Initial revision.
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
#include "dsm.h"
#include "bt.h"
#include "btpfi.h"
#include "oi_status.h"
#include "oi_l2cap.h"
#include "oi_l2cap_group.h"
#include "oi_l2cap_qos.h"
#include "oi_l2cap_mbuf.h"
#include "oi_memmgr.h"
#include "btmsg.h"
#include "bti.h"
#include "btpfcs.h"
#include "btcmdi.h"
#include "btrci.h"
#include "btpfcmdi.h"
#include "btpfgoepi.h"
#include "btsd.h"
#include "btdebug.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/* Maximum number of L2 connections this can map at once                   */
/*-------------------------------------------------------------------------*/
#define NUM_L2IF_ENTRIES 10

/*-------------------------------------------------------------------------*/
/* Check command status of bt driver commands                              */
/*-------------------------------------------------------------------------*/
#define BT_CMD(cmd,status) if( ((status) = (cmd)) == BT_CS_GN_PENDING){(status)=OI_OK;}

/*=========================================================================*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*=========================================================================*/

/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the Profile L2 layer.    */
/*-------------------------------------------------------------------------*/

#define BT_PF_L2IF_NUM_EVENTS    30
LOCAL bt_pf_event_q_info_type  bt_pf_l2if_ev_q_info;
LOCAL q_type                   bt_pf_l2if_ev_q;


/*-------------------------------------------------------------------------*/
/* Structure to keep track of all L2 information per CID                   */
/* These are used for mobile originatied connections, and to route events  */
/* for those connections back to the correct callback functions.           */
/*-------------------------------------------------------------------------*/
typedef struct {
  bt_app_id_type                     aid;
  uint16                             cid;
  uint16                             psm;
  bt_bd_addr_type                    bd_addr;
  word                               in_mtu;
  word                               out_mtu;
  word                               in_flush_to;
  bt_l2_qos_type                     in_qos;
  OI_L2CAP_DISCONNECT_IND            disconnect_ind;
  OI_L2CAP_RECV_DATA_IND             recv_ind;
  OI_L2CAP_DATA_WRITE_CFM            write_cb;
  OI_L2CAP_CONNECT_COMPLETE_CALLBACK connect_cb;
  OI_L2CAP_CONNECT_IND               connect_ind;
  L2CAP_WRITE_MBUF_CFM               mbuf_write_cfm_cb;
  dsm_item_type                     *pending_dsm;
} bt_pf_l2if_db_type;

LOCAL bt_pf_l2if_db_type bt_pf_l2if_db[NUM_L2IF_ENTRIES] = {0};

/*=========================================================================*/
/*                                                                         */
/*            INTERNAL FUNCTIONS                                           */
/*                                                                         */
/*=========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_l2if_store_bt_event

DESCRIPTION
  Stores a specified BT event on the Profile L2 Interface BT event queue.

===========================================================================*/
LOCAL void bt_pf_l2if_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  bt_pf_store_bt_event( bt_ev_ptr, &bt_pf_l2if_ev_q_info );

}

/*===========================================================================

FUNCTION
  bt_pf_l2if_lookup_by_cid

DESCRIPTION
  Returns a pointer to a bt_pf_l2if_db_type, or NULL if nothing is found.

===========================================================================*/
LOCAL bt_pf_l2if_db_type* bt_pf_l2if_lookup_by_cid( uint16 cid )
{
  int i;
  for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
  {
    if( bt_pf_l2if_db[i].cid == cid )
    {
      return &bt_pf_l2if_db[i];
    }
  }
  return NULL;
}

/*===========================================================================

FUNCTION
   bt_pf_l2if_get_new_db_entry

DESCRIPTION
  Returns a pointer to a  bt_pf_l2if_db_type matching the next available empty
  location. Returns NULL if the table is full.

===========================================================================*/
bt_pf_l2if_db_type* bt_pf_l2if_get_new_db_entry( bt_app_id_type app_id )
{
  int i;
  bt_pf_l2if_db_type* rv = NULL;
  for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
  {
    if( bt_pf_l2if_db[i].aid == BT_APP_ID_NULL )
    {
      if( app_id == BT_APP_ID_NULL )
      {
        bt_pf_l2if_db[i].aid = bt_cmd_ec_get_app_id_and_register( bt_pf_l2if_store_bt_event );
      }
      else
      {
        bt_pf_l2if_db[i].aid = app_id;
      }
      if( bt_pf_l2if_db[i].aid != BT_APP_ID_NULL )
      {
        rv = &bt_pf_l2if_db[i];
      }
      break;
    }
  }
  return rv;
}

/*===========================================================================

FUNCTION
  bt_pf_l2if_clear_db_entry

DESCRIPTION
  Clears relevent info in a DB entry so it can be re-used

===========================================================================*/
void bt_pf_l2if_clear_db_entry( bt_pf_l2if_db_type* entry )
{
  BT_MSG_DEBUG("BT PF L2: Clearing DB Entry AID %x, CID %x",
               entry->aid, entry->cid, 0 );

  entry->cid = BT_L2_NULL_CID;

  if( entry->psm != BT_L2_PSM_INVALID )
  {
    // This entry belongs to a registered sever, don't delete it.
    return;
  }

  if( entry->aid != BT_APP_ID_NULL )
  {
    int i;
    int num_entries_with_aid = 0;
    for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
    {
      if( bt_pf_l2if_db[i].aid == entry->aid )
      {
        num_entries_with_aid++;
      }
    }
    if( num_entries_with_aid == 1 )
    {
      bt_cmd_ec_free_application_id( entry->aid );
    }
  }

  dsm_free_packet( &entry->pending_dsm );

  memset( entry, 0, sizeof(bt_pf_l2if_db_type));
  entry->aid = BT_APP_ID_NULL;
}


/*===========================================================================

FUNCTION
  bt_pf_l2if_lookup_by_psm

DESCRIPTION
  Returns a pointer to a bt_pf_l2if_db_type, or NULL if nothing is found.

===========================================================================*/
LOCAL bt_pf_l2if_db_type* bt_pf_l2if_lookup_by_psm( uint16 psm )
{
  int i;
  for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
  {
    if( bt_pf_l2if_db[i].psm == psm )
    {
      return &bt_pf_l2if_db[i];
    }
  }
  return NULL;
}

/*===========================================================================

FUNCTION
  bt_pf_l2if_lookup_by_aid

DESCRIPTION
  Returns a pointer to a bt_pf_l2if_db_type, or NULL if nothing is found.

===========================================================================*/
LOCAL bt_pf_l2if_db_type* bt_pf_l2if_lookup_by_aid( bt_app_id_type aid )
{
  int i;
  for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
  {
    if( bt_pf_l2if_db[i].aid == aid )
    {
      return &bt_pf_l2if_db[i];
    }
  }
  return NULL;
}

/*===========================================================================

FUNCTION
  free_db_entry_by_psm

DESCRIPTION
  Free's a given entry from the PSM DB.

===========================================================================*/
LOCAL void free_db_entry_by_psm( uint16 psm )
{
  int i;
  for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
  {
    if( bt_pf_l2if_db[i].psm == psm )
    {
      bt_pf_l2if_db[i].psm = BT_L2_PSM_INVALID;
      bt_pf_l2if_clear_db_entry( &bt_pf_l2if_db[i] );
      break;
    }
  }
}

#ifdef FEATURE_BT_AVSYNC
/*===========================================================================

FUNCTION
  bt_pf_l2if_write_cfm

DESCRIPTION
  Write Confirm Callback called when a l2cap packet has been transmitted 
  or flushed by the baseband.
===========================================================================*/
void bt_pf_l2if_write_cfm(struct bt_ev_msg_struct* ev_msg_ptr)
{
  bt_pf_a2dp_frame_handle_type*   frame_handle;
  bt_pf_l2if_db_type*             db_handle;

  frame_handle = ev_msg_ptr->ev_msg.ev_l2_tx_data_cfm.up_handle_ptr;
  if(frame_handle == NULL)
  {
    return;
  }
  db_handle = bt_pf_l2if_lookup_by_cid(frame_handle->l2cap_cid);

  if((db_handle != NULL) && (db_handle->mbuf_write_cfm_cb != NULL))
  {
    db_handle->mbuf_write_cfm_cb(frame_handle->l2cap_cid,
                                 frame_handle->mbuf, OI_OK);
  }
}
#endif /* FEATURE_BT_AVSYNC */

/*===========================================================================

FUNCTION
  bt_pf_l2if_process_event

DESCRIPTION
  Event routine for all events.

===========================================================================*/
LOCAL void bt_pf_l2if_process_event( struct bt_ev_msg_struct* ev_msg_ptr )
{
  bt_pf_l2if_db_type*     db_handle;
  dsm_item_type*          dsm_item_ptr;

  switch (ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
      if( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
             BT_CS_GN_SUCCESS )
      {
        BT_MSG_DEBUG( "BT PF L2 RX EV: Cmd Done, Status: %x, App: %x, Cmd: %x",
                      ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                      ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id,
                      ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type );
      }

      /* Clean up if this was a failed connection attempt */
      if( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status != BT_CS_GN_SUCCESS &&
          ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type == BT_CMD_L2_CONNECT_IMMEDIATE_CID )
      {
        // TODO: Figure out which entry in the bt_pf_l2if_db is connecting, and
        // clear out the info.
        // Also, call the connect callback and let them know it failed
        BT_ERR("BT PF L2IF: failed connect command",0,0,0);
      }
      break;
    case BT_EV_L2_CONNECTED:
      BT_MSG_DEBUG( "BT PF L2 RX EV: L2 Connected, App: %x, CID: %x Out MTU: %x",
                    ev_msg_ptr->ev_hdr.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_l2_conn.cid,
		    ev_msg_ptr->ev_msg.ev_l2_conn.mtu );
     // BT_BDA( MSG_DEBUG, "BT PF L2: Connected", &ev_msg_ptr->ev_msg.ev_l2_conn.bd_addr );
      db_handle = bt_pf_l2if_lookup_by_cid(ev_msg_ptr->ev_msg.ev_l2_conn.cid);
      if( db_handle != NULL )
      {
        /* This is a response to a connection request */
        db_handle->out_mtu = ev_msg_ptr->ev_msg.ev_l2_conn.mtu;
        db_handle->bd_addr = ev_msg_ptr->ev_msg.ev_l2_conn.bd_addr;
        if( db_handle->connect_cb )
        {
          bt_cmd_l2_disable_timeout( db_handle->aid, db_handle->cid );
          db_handle->connect_cb( db_handle->cid,
                                 db_handle->in_mtu,
                                 db_handle->out_mtu,
                                 OI_OK );
        }
      }
      else if( (db_handle = bt_pf_l2if_lookup_by_aid(ev_msg_ptr->ev_hdr.bt_app_id)) != NULL )
      {
        uint16 psm = db_handle->psm;

        /* This is a new incoming connection */
        if( db_handle->cid != BT_L2_NULL_CID )
        {
          // This entry in the table is already associated with a connection, so
          // get a new entry, and copy some data
          bt_pf_l2if_db_type *new_db_handle;
          new_db_handle = bt_pf_l2if_get_new_db_entry( db_handle->aid );
          if( new_db_handle )
          {
            memcpy( new_db_handle, db_handle, sizeof( bt_pf_l2if_db_type ) );
            new_db_handle->psm = BT_L2_PSM_INVALID;
          }
          db_handle = new_db_handle;
        }

        if( db_handle )
        {
          db_handle->out_mtu = ev_msg_ptr->ev_msg.ev_l2_conn.mtu;
          db_handle->bd_addr = ev_msg_ptr->ev_msg.ev_l2_conn.bd_addr;
          db_handle->cid     = ev_msg_ptr->ev_msg.ev_l2_conn.cid;

          if( db_handle->connect_ind )
          {
            bt_cmd_l2_disable_timeout( db_handle->aid, db_handle->cid );
            db_handle->connect_ind( psm,
                                    (OI_BD_ADDR*)&db_handle->bd_addr,
                                    db_handle->cid );
          }
        }
        else
        {
          BT_ERR("BT PF L2IF: Can't send con_ind; disconnecting",
                 0,0,0);
          bt_cmd_l2_disconnect( ev_msg_ptr->ev_hdr.bt_app_id,
                                ev_msg_ptr->ev_msg.ev_l2_conn.cid );
        }
      }
      else
      {
        BT_ERR("BT PF L2: no route for connection event",0,0,0);
      }
      break;
    case BT_EV_L2_CONNECTION_FAILED:
      BT_MSG_DEBUG( "BT PF L2 RX EV: L2 Connect failed, App: %x",
                    ev_msg_ptr->ev_hdr.bt_app_id, 0, 0);
      db_handle = bt_pf_l2if_lookup_by_cid(ev_msg_ptr->ev_msg.ev_l2_conn_failed.cid);
      if( db_handle != NULL )
      {
        if( db_handle->connect_cb )
        {
          db_handle->connect_cb( db_handle->cid,
                                 db_handle->in_mtu,
                                 db_handle->out_mtu,
                                 bt_pf_map_event_code(ev_msg_ptr->
                                                      ev_msg.ev_l2_conn_failed.reason) );
        }
        bt_pf_l2if_clear_db_entry( db_handle );
      }
      else
      {
        BT_ERR("BT PF L2IF: Got con failed for unk cid %x",
               ev_msg_ptr->ev_msg.ev_l2_conn_failed.cid,0,0);
      }
      break;
    case BT_EV_L2_RX_DATA:
      BT_MSG_DEBUG( "BT PF L2 RX EV: L2 rx data, App: %x, CID: %x, bytes: %x",
                    ev_msg_ptr->ev_hdr.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_l2_rxd.cid,
		    dsm_length_packet( ev_msg_ptr->ev_msg.ev_l2_rxd.dsm_ptr ) );
      db_handle = bt_pf_l2if_lookup_by_cid(ev_msg_ptr->ev_msg.ev_l2_rxd.cid);
      dsm_item_ptr = ev_msg_ptr->ev_msg.ev_l2_rxd.dsm_ptr;
      if( db_handle != NULL )
      {
	if ( db_handle->recv_ind )
	{
	  int16 length;
	  void  *buf;

	  length = dsm_length_packet(ev_msg_ptr->ev_msg.ev_l2_rxd.dsm_ptr);
	  buf = OI_Malloc( length );
	  if( buf == NULL )
	  {
	    BT_ERR("BT PF L2IF: Can't allocate memory for RX data",0,0,0);
	    dsm_free_packet( &dsm_item_ptr );
	  }
	  else
	  {
	    dsm_pullup( &dsm_item_ptr, buf, length );
	    db_handle->recv_ind( db_handle->cid, buf, length );
	    OI_Free( buf );
	  }
	}
	else
	{
	  BT_MSG_DEBUG("BT PF L2IF: Saving rx data for later",0,0,0);
	  if( db_handle->pending_dsm != NULL )
	  {
	    BT_ERR("BT PF L2IF: Dropping RX L2 data cid %x",
		   ev_msg_ptr->ev_msg.ev_l2_rxd.cid, 0, 0);
	    dsm_free_packet( &dsm_item_ptr );
	  }
	  else
	  {
	    db_handle->pending_dsm = dsm_item_ptr;
	  }
	}
      }
      else
      {
        dsm_free_packet( &dsm_item_ptr );
        BT_ERR( "BT PF L2: Can't find route for RX data!", 0,0,0 );
      }
      break;
    case BT_EV_L2_RECONFIGURED:
      BT_MSG_DEBUG( "BT PF L2 RX EV: L2 Reconfigured, cid %x out_mtu %x in flush to %x",
                    ev_msg_ptr->ev_msg.ev_l2_reconfig.cid,
		    ev_msg_ptr->ev_msg.ev_l2_reconfig.out_mtu,
		    ev_msg_ptr->ev_msg.ev_l2_reconfig.in_flush_to );
      db_handle = bt_pf_l2if_lookup_by_cid(ev_msg_ptr->ev_msg.ev_l2_reconfig.cid);
      if( db_handle && db_handle->connect_cb )
      {
        db_handle->out_mtu     = ev_msg_ptr->ev_msg.ev_l2_reconfig.out_mtu;
        db_handle->in_flush_to = ev_msg_ptr->ev_msg.ev_l2_reconfig.in_flush_to;
        db_handle->in_qos      = ev_msg_ptr->ev_msg.ev_l2_reconfig.in_qos;
        db_handle->connect_cb( db_handle->cid,
                               db_handle->in_mtu,
                               db_handle->out_mtu,
                               bt_pf_map_event_code(ev_msg_ptr->
                                                    ev_msg.ev_l2_reconfig.reason) );
      }
      break;

#ifdef FEATURE_BT_AVSYNC
    case BT_EV_L2_TX_DATA_CFM:
      bt_pf_l2if_write_cfm(ev_msg_ptr);
      break;
#endif /* FEATURE_BT_AVSYNC */

    case BT_EV_L2_DISCONNECTED:
      BT_MSG_DEBUG( "BT PF L2 RX EV: L2 disconnected, App: %x, CID %x",
                    ev_msg_ptr->ev_hdr.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_l2_disc.cid, 0);
      db_handle = bt_pf_l2if_lookup_by_cid(ev_msg_ptr->ev_msg.ev_l2_disc.cid);
      if( db_handle )
      {
        if( db_handle->disconnect_ind )
        {
          db_handle->disconnect_ind( db_handle->cid,
                                     bt_pf_map_event_code(ev_msg_ptr->
                                                          ev_msg.ev_l2_disc.reason) );
        }
        bt_pf_l2if_clear_db_entry( db_handle );
      }
      else
      {
        BT_ERR("BT PF L2IF: can't find entry for discon cid %x, aid %x",
               ev_msg_ptr->ev_msg.ev_l2_disc.cid,
               ev_msg_ptr->ev_hdr.bt_app_id, 0 );
      }
      break;
    case BT_EV_L2_PING: /* no break */
    default:
      BT_ERR( "BT PF L2: unknown event type %x",
              ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      break;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_l2if_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by the Profile L2 interface.

===========================================================================*/
uint16 bt_pf_l2if_get_max_event_bytes
(
  void
)
{

  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_connected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_rx_data_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_disconnected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_connection_failed_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_reconfigured_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );

}

/*===========================================================================

FUNCTION
  bt_pf_l2if_process_ev_queue

DESCRIPTION
  Dequeues events on the Profile L2 interface event queue and processes them.

===========================================================================*/
void bt_pf_l2if_process_ev_queue( void )
{

  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_pf_l2if_ev_q );

  BT_VSTAT_MAX( q_count, PF_L2IF_MAX_EV_Q_DEPTH );

  while ( (bt_ev_ptr = (bt_ev_msg_type *)q_get( &bt_pf_l2if_ev_q )) != NULL )
  {
    bt_pf_l2if_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}

/*===========================================================================

           FUNCTIONS FOR MAPPING L2CAP APIs

===========================================================================*/

/*===========================================================================

FUNCTION
  OI_L2CAP_Disconnect

DESCRIPTION
  Disconnect an L2CAP connection

===========================================================================*/
OI_STATUS OI_L2CAP_Disconnect(OI_L2CAP_CID cid)
{
  OI_STATUS status;
  bt_pf_l2if_db_type *db_handle;

  BT_MSG_DEBUG( "BT PF L2 CMD RX: Disconnect CID %x", cid, 0, 0 );

  db_handle = bt_pf_l2if_lookup_by_cid( cid );
  if( db_handle )
  {
    BT_CMD( bt_cmd_l2_disconn_immediate( db_handle->aid, cid ), status );
  }
  else
  {
    status = OI_STATUS_INVALID_CID;
  }

  return status;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_Accept

DESCRIPTION
  This function initiates the accepting (or rejecting of a connection.

===========================================================================*/
OI_STATUS OI_L2CAP_Accept(OI_L2CAP_CONNECT_COMPLETE_CALLBACK confirmCB,
                          OI_L2CAP_DISCONNECT_IND disconnectInd,
                          OI_L2CAP_RECV_DATA_IND recvInd,
                          OI_UINT16 lcid,
                          OI_L2CAP_CONNECT_PARAMS *connectSpec,
                          OI_BOOL accept,
                          const OI_CONNECT_POLICY *policy)
{
  OI_STATUS status;
  bt_pf_l2if_db_type *db_handle;

  BT_MSG_DEBUG( "BT PF L2 CMD RX: Accept %x CID %x", accept, lcid, 0 );

  db_handle = bt_pf_l2if_lookup_by_cid( lcid );
  if( db_handle == NULL )
  {
    /* Where did this connection go? */
    status = OI_STATUS_INVALID_CID;
    BT_ERR( "BT PF L2: No information about CID %d",lcid,0,0);
  }
  else if( accept == TRUE )
  {
    status = OI_OK;

    db_handle->disconnect_ind = disconnectInd;
    db_handle->recv_ind       = recvInd;
    db_handle->connect_cb     = confirmCB;

    /* Call the confirm CB right away: */
    confirmCB( lcid, db_handle->in_mtu, db_handle->out_mtu, OI_OK );

    /* If there is data pending, send it now */
    if( db_handle->pending_dsm && recvInd != NULL )
    {
      int16 length;
      void  *buf;

      length = dsm_length_packet(db_handle->pending_dsm);
      buf = OI_Malloc( length );
      if( buf == NULL )
      {
        BT_ERR("BT PF L2IF: Can't allocate memory for RX data",0,0,0);
        dsm_free_packet( &db_handle->pending_dsm );
      }
      else
      {
        dsm_pullup( &db_handle->pending_dsm, buf, length );
        db_handle->recv_ind( db_handle->cid, buf, length );
        OI_Free( buf );
      }
    }
  }
  else /* accept == FALSE */
  {
    /* Reject the connection */
    /* We don't exactly support this. But we can disconnect after the fact */
    BT_CMD( bt_cmd_l2_disconnect( db_handle->aid, lcid ), status );
    dsm_free_packet( &db_handle->pending_dsm );
  }
  return status;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_Write

DESCRIPTION
  This function writes to the specified L2 connection

===========================================================================*/
OI_STATUS OI_L2CAP_Write(OI_L2CAP_DATA_WRITE_CFM callback,
                         OI_L2CAP_CID cid,
                         OI_UINT8 *buf,
                         OI_UINT16 length)
{
  OI_STATUS status;
  bt_pf_l2if_db_type* handle;
  dsm_item_type* dsm_ptr;

  BT_MSG_DEBUG( "BT PF L2: Write", 0, 0, 0 );

  handle = bt_pf_l2if_lookup_by_cid( cid );
  if( handle == NULL )
  {
    status = OI_STATUS_INTERNAL_ERROR;
    BT_MSG_HIGH( "BT PF L2: Writing to nonexistant CID %d", cid, 0, 0 );
  }
  else
  {
    uint16 bytes_pushed;

    handle->write_cb = callback;

    dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP, length );
    bytes_pushed = dsm_pushdown_tail( &dsm_ptr,
                                      buf,
                                      length,
                                      DSM_DS_POOL_SIZE( length ) );
    if( bytes_pushed != length )
    {
      status = OI_STATUS_INTERNAL_ERROR;
      BT_ERR( "BT PF L2: Write error filling DSM", 0,0,0 );
    }
    else
    {
      OI_Free( buf );
      BT_CMD(bt_cmd_l2_write( handle->aid, cid, dsm_ptr, NULL ),status);
    }
  }
  return status;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_Register

DESCRIPTION
  Registers with the L2cap interface

===========================================================================*/
OI_STATUS OI_L2CAP_Register(OI_L2CAP_PSM psm,
                            OI_L2CAP_CONNECT_IND connectInd,
                            const OI_L2CAP_CONNECT_PARAMS *params)
{
  OI_STATUS status;
  bt_pf_l2if_db_type *db_handle;
  uint16             out_flush_to = BT_L2_DEFAULT_FLUSH_TIMEOUT;
  uint16             in_mtu       = BT_L2_DEFAULT_MTU;

  BT_MSG_DEBUG( "BT PF L2 CMD RX: Register", 0, 0, 0 );

  db_handle = bt_pf_l2if_get_new_db_entry( BT_APP_ID_NULL );
  if( db_handle == NULL )
  {
    status = OI_STATUS_NO_RESOURCES;
  }
  else
  {
    /* Really these values in db_handle shouldn't be set until we get the cmd_done from L2
     * for the set_params command */
    db_handle->psm = psm;
    db_handle->connect_ind = connectInd;
    db_handle->in_mtu = BT_L2_DEFAULT_MTU;

    /* QCom's L2 layer requires that we specify the L2 parameters here.
       OI Code doesn't specify these until the l2if_accept/connect calls.
       So register with default values here, and hope it works */

    BT_MSG_API( "BT PF L2 CMD TX: L2 StPrm PSM %x AID %x",
                psm, db_handle->aid, 0 );
    if(params)
    {
      if(params->inMTU != 0)
      {
         in_mtu = params->inMTU;
      }
      if(params->outFlushTO != 0)
      {
         out_flush_to = params->outFlushTO;
      }
    }
    BT_CMD( bt_cmd_l2_set_params( db_handle->aid, BT_L2_NULL_CID, psm,
                                  out_flush_to, in_mtu,
                                  BT_QOS_BEST_EFFORT,              BT_L2_DEFAULT_TOKEN_RATE,
                                  BT_L2_DEFAULT_TOKEN_BUCKET_SIZE, BT_L2_DEFAULT_PEAK_BANDWIDTH,
                                  BT_L2_DEFAULT_LATENCY,           BT_L2_DEFAULT_DELAY_VARIATION ),
            status );

    if( status == OI_OK )
    {
      BT_CMD( bt_cmd_l2_register( db_handle->aid, psm ), status );
    }
  }
  if( db_handle && (status != OI_OK || db_handle->aid == BT_APP_ID_NULL ) )
  {
    db_handle->psm = BT_L2_PSM_INVALID;
    bt_pf_l2if_clear_db_entry( db_handle );
  }
  return status;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_Deregister

DESCRIPTION
  Deregister with L2CAP

===========================================================================*/
OI_STATUS OI_L2CAP_Deregister(OI_L2CAP_PSM psm)
{
  OI_STATUS status;
  bt_pf_l2if_db_type *db_handle;
  boolean             already_connected = FALSE;
  int                 i;

  BT_MSG_DEBUG( "BT PF L2: Deregister", 0, 0, 0 );

  db_handle = bt_pf_l2if_lookup_by_psm( psm );
  if( db_handle )
  {
    /* To better emulate OI's L2 layer, don't allow deregistration
     * if there are current connections */
    for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
    {
        if( (bt_pf_l2if_db[i].aid == db_handle->aid) &&
            (bt_pf_l2if_db[i].cid != BT_L2_NULL_CID) )
        {
          already_connected = TRUE;
        }  
    }
    if( already_connected == FALSE )
    {
      BT_CMD( bt_cmd_l2_unregister( db_handle->aid, psm ), status );
      free_db_entry_by_psm( psm );
    }
    else
    {
      status = OI_STATUS_STILL_CONNECTED;
    }
  }
  else
  {
    status = OI_STATUS_NOT_REGISTERED;
  }
  return status;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_Connect

DESCRIPTION
  This function initiates a connection to a remote device by:
    - initializing the connection parameters
    - requesting an ACL connection to the remote device

  The connection MTU is determined in one of two ways.  In the first,
  connectSpec is not used and the MTU is the minimum of the L2CAP
  default and the value in the stack config.  In the second,
  connectSpec is used and the MTU is the minimum of connectSpec->inMTU
  and the value in the stack config.

===========================================================================*/
OI_STATUS OI_L2CAP_Connect(OI_L2CAP_CONNECT_COMPLETE_CALLBACK confirmCB,
                           OI_L2CAP_DISCONNECT_IND disconnectInd,
                           OI_L2CAP_RECV_DATA_IND recvInd,
                           OI_L2CAP_PSM targetPSM,
                           OI_BD_ADDR *addr,
                           OI_L2CAP_CONNECT_PARAMS *connectSpec,
                           OI_L2CAP_CID *cid,
                           const OI_CONNECT_POLICY *policy)
{
  /* TODO: Use the correct info from the policy */

  OI_STATUS status;
  bt_pf_l2if_db_type *db_handle;
  bt_pf_l2if_db_type *existing_psm_handle;
  bt_app_id_type app_id;

  BT_MSG_DEBUG( "BT PF L2 CMD RX: Connect PSM %x", targetPSM, 0, 0 );
  //BT_BDA( MSG_DEBUG, "BT PF L2: Connect", (bt_bd_addr_type*)addr );

  db_handle = bt_pf_l2if_get_new_db_entry( BT_APP_ID_NULL );

  existing_psm_handle = bt_pf_l2if_lookup_by_psm( targetPSM );
  if( existing_psm_handle )
  {
    /* The current underlying L2 layer is unable to deal with
     * having one app registered with a PSM, and another app try
     * to connect to a remote device with the same PSM. So use
     * the same AppID here for both */
    if( db_handle )
    {
      bt_cmd_ec_free_application_id( db_handle->aid );
      db_handle->aid = existing_psm_handle->aid;
    }
  }
  if( db_handle )
  {
    app_id = db_handle->aid;

    BT_CMD( bt_cmd_l2_set_params( app_id, BT_L2_NULL_CID, targetPSM,
                                  connectSpec->outFlushTO,
                                  connectSpec->inMTU,
                                  /* QoS values are not used: */
                                  BT_QOS_BEST_EFFORT,              BT_L2_DEFAULT_TOKEN_RATE,
                                  BT_L2_DEFAULT_TOKEN_BUCKET_SIZE, BT_L2_DEFAULT_PEAK_BANDWIDTH,
                                  BT_L2_DEFAULT_LATENCY,           BT_L2_DEFAULT_DELAY_VARIATION ),
            status );
    if( status == OI_OK )
    {
      BT_CMD( bt_cmd_l2_connect_immediate_cid( app_id,
                                               targetPSM,
                                               (bt_bd_addr_type*)addr,
                                               cid ),
              status );
      if( status == OI_OK )
      {
        db_handle->cid = *cid;
        db_handle->connect_cb = confirmCB;
        db_handle->disconnect_ind = disconnectInd;
        db_handle->recv_ind = recvInd;
        db_handle->in_mtu = connectSpec->inMTU;
        BT_MSG_DEBUG("BT PF L2 Connect: Connecting with CID: %x", *cid,0,0);
      }
    }
  }
  else
  {
    // Couldn't get a new CID entry. The table must be full.
    status = OI_STATUS_NO_RESOURCES;
    BT_ERR( "BT PF L2: Connect, can't get cid entry",0,0,0);
  }
  return status;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_WriteMBUF

DESCRIPTION
  Writes an MBUF to an open L2 connection. Since the lower layer has no
  mechanism of informing us when the data was sent, call the callback
  immediately.

===========================================================================*/
OI_STATUS OI_L2CAP_WriteMBUF(L2CAP_WRITE_MBUF_CFM callback,
                             OI_L2CAP_CID cid,
                             OI_MBUF *mbuf,
                             void *frame_handle)
{
  OI_STATUS status = BT_CS_PF_INVALID_CID;
  bt_pf_l2if_db_type* db_handle = bt_pf_l2if_lookup_by_cid( cid );
#ifdef FEATURE_BT_AVSYNC
  bt_pf_a2dp_frame_handle_type  *up_handle = 
                                (bt_pf_a2dp_frame_handle_type *)frame_handle;
#endif /* FEATURE_BT_AVSYNC */
  if( db_handle )
  {
    mbuf->dsm_state = BT_PF_MBUF_DSM_SENT;
    BT_CMD( bt_cmd_l2_write( db_handle->aid, cid, mbuf->dsm_ptr, frame_handle ), status );
  }

#ifdef FEATURE_BT_AVSYNC
  if(((NULL != frame_handle) && (BT_INVALID_ADDRESS != frame_handle))
                        && (NULL != callback) && (NULL != db_handle))
  {
    /* Store the call back parameters for the transmit complete call back */
    up_handle->l2cap_cid = cid;
    up_handle->mbuf = mbuf;
    db_handle->mbuf_write_cfm_cb = callback;
  }
  else if((NULL != callback) && (NULL != db_handle) && (frame_handle == NULL))
  {
    /* If is it a Non-media Packet, then generate a cfm immediately */
#ifdef FEATURE_BT_AVSYNC_DEBUG
    BT_MSG_HIGH( " BT PF L2IF: Non Media packet requiring a TxCfm", 0, 0, 0);
#endif
    callback(cid, mbuf, status);
  }
  else
  {
    /* There is no need to generate Tx Cfm, As it is a media 
     * packet that dosent not need a Cfm, Just free the mbuf 
     */
    OI_MBUF_Free(mbuf); 
  }
#else
  callback( cid, mbuf, status );
#endif /* FEATURE_BT_AVSYNC */
  return status;
}

/*=========================================================================*/
/*                                                                         */
/*            UNIMPLEMENTED FUNCTIONS - stubs only                         */
/*                                                                         */
/*=========================================================================*/

/*===========================================================================

FUNCTION
  OI_L2CAP_GroupAddMember

DESCRIPTION
  Not implemented

===========================================================================*/
OI_STATUS OI_L2CAP_GroupAddMember(OI_L2CAP_GROUP_ADD_MEMBER_CFM callback,
                                  OI_L2CAP_CID cid,
                                  OI_BD_ADDR *addr)
{
  // Not implemented.
  BT_ERR("BT PF L2: non-implemnted function called: OI_L2CAP_GroupAddMember",
               0,0,0);

  /* The proper response is to return NOT IMPLEMENTED. However, the OI stack
   * seems to stop working if we do that. So just return OI_OK here. */
  //  return OI_STATUS_NOT_IMPLEMENTED;
  return OI_OK;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_GroupRemoveMember

DESCRIPTION
  Not implemented

===========================================================================*/
OI_STATUS OI_L2CAP_GroupRemoveMember(OI_L2CAP_CID cid,
                                     OI_BD_ADDR *addr)
{
  // Not implemented.
  BT_ERR("BT PF L2: non-implemnted function called: OI_L2CAP_GroupRemoveMember",
               0,0,0);
  /* The proper response is to return NOT IMPLEMENTED. However, the OI stack
   * seems to stop working if we do that. So just return OI_OK here. */
  //  return OI_STATUS_NOT_IMPLEMENTED;
  return OI_OK;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_GroupClose

DESCRIPTION
  Not implemented

===========================================================================*/
OI_STATUS OI_L2CAP_GroupClose(OI_L2CAP_CID cid)
{
  // Not implemented.
  BT_ERR("BT PF L2: non-implemnted function called: OI_L2CAP_GroupClose",
               0,0,0);
  /* The proper response is to return NOT IMPLEMENTED. However, the OI stack
   * seems to stop working if we do that. So just return OI_OK here. */
  //  return OI_STATUS_NOT_IMPLEMENTED;
  return OI_OK;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_IsGroupCID

DESCRIPTION
  No CIDs are group CIDs. Always returns false.

===========================================================================*/
OI_BOOL OI_L2CAP_IsGroupCID( OI_L2CAP_CID cid )
{
  return FALSE;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_EnableCLT

DESCRIPTION
  Not implemented

===========================================================================*/
OI_STATUS OI_L2CAP_EnableCLT(OI_L2CAP_PSM psm,
                             OI_L2CAP_CL_DATA_RECV readCB)
{
  // Not implemented.
  BT_MSG_HIGH("BT PF L2: non-implemnted function called: OI_L2CAP_EnableCLT",
              0,0,0);
  /* The proper response is to return NOT IMPLEMENTED. However, the OI stack
   * seems to stop working if we do that. So just return OI_OK here. */
  // return OI_STATUS_NOT_IMPLEMENTED;
  return OI_OK;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_DisableCLT

DESCRIPTION
  Not implemented

===========================================================================*/
OI_STATUS OI_L2CAP_DisableCLT(OI_L2CAP_PSM psm)
{
  // Not implemented.
  BT_MSG_HIGH("BT PF L2: non-implemnted function called: OI_L2CAP_EnableCLT",
              0,0,0);
  /* The proper response is to return NOT IMPLEMENTED. However, the OI stack
   * seems to stop working if we do that. So just return OI_OK here. */
  //  return OI_STATUS_NOT_IMPLEMENTED;
  return OI_OK;
}

/*===========================================================================

FUNCTION
  OI_L2CAP_GroupCreate

DESCRIPTION
  Not implemented

===========================================================================*/
OI_STATUS OI_L2CAP_GroupCreate(OI_L2CAP_PSM psm,
                               OI_L2CAP_CID *cid,
                               OI_L2CAP_GROUP_REMOVE_MEMBER_CALLBACK removeCB)
{
  // Not implemented.
  BT_ERR("BT PF L2: non-implemnted function called: OI_L2CAP_GroupCreate",
               0,0,0);
  /* The proper response is to return NOT IMPLEMENTED. However, the OI stack
   * seems to stop working if we do that. So just return OI_OK here. */
  //  return OI_STATUS_NOT_IMPLEMENTED;
  return OI_OK;
}

/*=========================================================================*/
/*                                                                         */
/*            EXTERNAL FUNCTIONS                                           */
/*                                                                         */
/*=========================================================================*/
/* Init devmgr */
extern void bt_pf_l2if_init( void )
{
  int i;

  (void) q_init( &bt_pf_l2if_ev_q );
  bt_pf_l2if_ev_q_info.event_q_ptr      = &bt_pf_l2if_ev_q;
  bt_pf_l2if_ev_q_info.max_q_depth      = BT_PF_L2IF_NUM_EVENTS;
  bt_pf_l2if_ev_q_info.pf_task_sig      = BT_PF_L2IF_EV_SIG;
  bt_pf_l2if_ev_q_info.max_event_bytes  = bt_pf_l2if_get_max_event_bytes();

  for( i = 0; i < NUM_L2IF_ENTRIES; i++ )
  {
    memset( (void*)&bt_pf_l2if_db[i], 0, sizeof(bt_pf_l2if_db_type));
    bt_pf_l2if_db[i].cid = BT_L2_NULL_CID;
    bt_pf_l2if_db[i].psm = BT_L2_PSM_INVALID;
    bt_pf_l2if_db[i].aid = BT_APP_ID_NULL;
  }
}
#endif /* FEATURE_BT_EXTPF */
#endif /* FEATURE_BT */
