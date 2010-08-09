/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                                                                                         
         B L U E T O O T H    E X T E R N A L    P R O F I L E S
  O P E N   I N T E R F A C E   D E V M G R   G L U E   L A Y E R

GENERAL DESCRIPTION
  This module contains the glue code to implement the interface for
  OI's DEVMGR layer using QCom's underlying RM implementation.

Copyright (c) 2004 - 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfrmif.c#1 $ 

*
*    #8        21 Feb 2007           PN
* Fixed critical lint errors.
*
*    #7        14 Mar 2006            JH
* Modified to support updated bt_cmd_rm_connect_sco() command.
*
*    #6        06 Jul 2005          JTL
* Fixing bug in RegisterScoApplication.
*
*    #4        20 Jan 2005           GS
* Fixed typo in case BT_EV_RM_CONNECTION_FAILED_SCO in 
* bt_pf_rmif_process_event routine.
*
*    #2-3      10 Aug 2004          JTL
* Create interface for translating our RM handles to a number that the
* OI code likes.
*
*    #1        06 Aug 2004          JTL
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
#include "bt.h"
#include "btpfi.h"
#include "btmsg.h"
#include "bti.h"
#include "btpfcs.h"
#include "btcmdi.h"
#include "btpfcmdi.h"
#include "btsd.h"
#include "btdebug.h"

/* OI Header files */
#include "oi_status.h"
#include "oi_bt_spec.h"
#include "oi_devmgr.h"

/*=========================================================================*/
/*                                                                         */
/*                DEFINITIONS AND DECLARATIONS FOR MODULE                  */
/*                                                                         */
/* This section contains definitions for constants, macros, types,         */
/* variables and other items needed by this module.                        */
/*                                                                         */
/*=========================================================================*/

#define NUM_SCO_CLIENTS 3

/*-------------------------------------------------------------------------*/
/* Check command status of bt driver commands                              */
/*-------------------------------------------------------------------------*/
#define BT_CMD(cmd,status) if( ((status) = (cmd)) == BT_CS_GN_PENDING){(status)=OI_OK;}

/*-------------------------------------------------------------------------*/
/* Translate BT RM handles to OI sco handles                               */
/*-------------------------------------------------------------------------*/
#define BT_HANDLE_TO_OI_HANDLE(x) (x+1)
#define OI_HANDLE_TO_BT_HANDLE(x) (x-1)


/*=========================================================================*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*=========================================================================*/

/*-------------------------------------------------------------------------*/
/* App ID to use for outgoing SCO connections                              */
/*-------------------------------------------------------------------------*/
LOCAL bt_app_id_type bt_pf_rmif_aid = BT_APP_ID_NULL;

/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the Profile RMIF layer.*/
/*-------------------------------------------------------------------------*/

#define BT_PF_RMIF_NUM_EVENTS    30
LOCAL bt_pf_event_q_info_type  bt_pf_rmif_ev_q_info;
LOCAL q_type                   bt_pf_rmif_ev_q;

/*-------------------------------------------------------------------------*/
/* Structure to keep track of all SCO information per rm handle and BD Addr*/
/* RM Handles are used for mobile originatied connections, and to route    */
/* events for those connections back to the correct callback functions.    */
/*                                                                         */
/* BD Addrs are used for mobile terminated connections, and to route       */
/* events for those connecitons (the connection event) to the correct      */
/* callback function.                                                      */
/*-------------------------------------------------------------------------*/
typedef struct {
  bt_rm_handle_type   rm_handle;
  bt_bd_addr_type     bd_addr;

  OI_DEVMGR_SCO_CONNECT_COMPLETE_CB    connection_complete_cb;
  OI_DEVMGR_SCO_DISCONNECT_COMPLETE_CB disconnect_complete_cb;
  OI_DEVMGR_SCO_CONNECT_IND_CB         connect_ind_cb;
} bt_pf_sco_db_type;

LOCAL bt_pf_sco_db_type bt_pf_sco_db[NUM_SCO_CLIENTS] = {0};

/*-------------------------------------------------------------------------*/
/* For incoming SCO connections not associated with a BD addr, call these  */
/* defalut CB functions.                                                   */
/*-------------------------------------------------------------------------*/
LOCAL bt_pf_sco_db_type bt_pf_sco_default = {0};

LOCAL const bt_pf_sco_db_type bt_pf_null_sco_db_entry = {0};

/*=========================================================================*/
/*                                                                         */
/*            INTERNAL FUNCTIONS                                           */
/*                                                                         */
/*=========================================================================*/

/*===========================================================================
FUNCTION
  bt_pf_rmif_lookup_by_rm_handle

DESCRIPTION
  Returns a db entry by lookup up the RM handle. Returns NULL if none matches
===========================================================================*/
LOCAL bt_pf_sco_db_type* bt_pf_rmif_lookup_by_rm_handle( bt_rm_handle_type rm_handle )
{
  int i;
  for( i = 0; i < NUM_SCO_CLIENTS; i++ )
  {
    if( bt_pf_sco_db[i].rm_handle == rm_handle )
    {
      return &bt_pf_sco_db[i];
    }
  }
  return NULL;
}

/*===========================================================================
FUNCTION
  bt_pf_rmif_lookup_by_bd_addr

DESCRIPTION
  Returns a db entry by lookup up the BD Addr. Returns NULL if none matches
===========================================================================*/
LOCAL bt_pf_sco_db_type* bt_pf_rmif_lookup_by_bd_addr( const bt_bd_addr_type* bd_addr_ptr )
{
  int i;
  for( i = 0; i < NUM_SCO_CLIENTS; i++ )
  {
    if( memcmp((void*)(&bt_pf_sco_db[i].bd_addr), (void*)bd_addr_ptr, sizeof(bt_bd_addr_type)) == 0)
    {
      return &bt_pf_sco_db[i];
    }
  }
  return NULL;
}

/*===========================================================================

FUNCTION
  bt_pf_rmif_get_new_db_entry

DESCRIPTION
  Returns a pointer to a  bt_pf_sco_db_type matching the next available empty
  location. Returns NULL if the table is full.
  
===========================================================================*/
LOCAL bt_pf_sco_db_type* bt_pf_rmif_get_new_db_entry( void )
{
  int i;

  for( i = 0; i < NUM_SCO_CLIENTS; i++ )
  {
    if( memcmp( &bt_pf_sco_db[i], &bt_pf_null_sco_db_entry, sizeof(bt_pf_sco_db_type)) == 0 )
    {
      return &bt_pf_sco_db[i];
    }
  }
  return NULL;
}

/*===========================================================================

FUNCTION
  bt_pf_rmif_get_db_entry_by_cb

DESCRIPTION
  Lookup a db entry by callback pointer
  
===========================================================================*/
LOCAL bt_pf_sco_db_type* bt_pf_rmif_get_db_entry_by_cb( OI_DEVMGR_SCO_SIGNAL_CALLBACKS *callbacks )
{
  int i;
  for( i = 0; i < NUM_SCO_CLIENTS; i++ )
  {
    if( bt_pf_sco_db[i].connection_complete_cb == callbacks->scoConnectCompleteCb &&
        bt_pf_sco_db[i].disconnect_complete_cb == callbacks->scoDisconnectCompleteCb &&
        bt_pf_sco_db[i].connect_ind_cb         == callbacks->scoConnectIndCb )
    {
      return &bt_pf_sco_db[i];
    }
  }
  return NULL;
}

/*===========================================================================

FUNCTION
  bt_pf_rmif_free_db_entry

DESCRIPTION
  Zero out a db entry
  
===========================================================================*/
LOCAL void bt_pf_rmif_free_db_entry( bt_pf_sco_db_type *delete_me )
{
  if( delete_me )
  {
    memset( delete_me, 0, sizeof( bt_pf_sco_db_type ) );
  }
}


/*===========================================================================

FUNCTION
  db_is_empty

DESCRIPTION
  Returns TRUE if there are no entries in the DB
  
===========================================================================*/
LOCAL boolean db_is_empty( void )
{
  int i;
  for( i = 0; i < NUM_SCO_CLIENTS; i++ )
  {
    if( memcmp( &bt_pf_sco_db[i], &bt_pf_null_sco_db_entry, sizeof(bt_pf_sco_db_type) ) )
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*===========================================================================

FUNCTION
  bt_pf_rmif_process_event

DESCRIPTION
  function for all RM events sent to the RMIF glue layer.
  
===========================================================================*/
LOCAL void bt_pf_rmif_process_event( struct bt_ev_msg_struct* ev_msg_ptr )
{
  bt_pf_sco_db_type *sco_entry;

  switch( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      BT_MSG_DEBUG( "BT PF RMIF RX EV: Cmd Done, Status: %x, App: %x, Cmd: %x", 
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type );
      break;
    case BT_EV_RM_CONNECT_REQ_SCO:
      BT_MSG_DEBUG("BT PF RMIF EV RX: sco conn req",0,0,0);

      sco_entry = bt_pf_rmif_lookup_by_bd_addr( &ev_msg_ptr->ev_msg.ev_rm_creqs.bd_addr );
      if( sco_entry == NULL )
      {
        if( (sco_entry = bt_pf_rmif_get_new_db_entry()) != NULL )
        {
          sco_entry->bd_addr = ev_msg_ptr->ev_msg.ev_rm_creqs.bd_addr;
          sco_entry->connection_complete_cb = bt_pf_sco_default.connection_complete_cb;
          sco_entry->disconnect_complete_cb = bt_pf_sco_default.disconnect_complete_cb;
          sco_entry->connect_ind_cb         = bt_pf_sco_default.connect_ind_cb;
        }
      }
      if( sco_entry )
      {
        if(sco_entry->connect_ind_cb)
        {
          uint32 class_of_device;
          class_of_device = 
            (ev_msg_ptr->ev_msg.ev_rm_creqs.class_of_device.cod_bytes[0])    |
            (ev_msg_ptr->ev_msg.ev_rm_creqs.class_of_device.cod_bytes[1]>>8) |
            (ev_msg_ptr->ev_msg.ev_rm_creqs.class_of_device.cod_bytes[2]>>16);

          if(sco_entry->connect_ind_cb( (OI_BD_ADDR*)(&sco_entry->bd_addr), 
                                        class_of_device )
             == TRUE )
          {
            /* OI doesn't have an analog for accept_sco, so we do that here */
            if( bt_cmd_rm_accept_sco( bt_pf_rmif_aid,
                                      &sco_entry->bd_addr )
                != BT_CS_GN_PENDING )
            {
              BT_ERR("BT PF RMIF: Can't accept SCO connection",0,0,0);
            }
          }
        }
        else
        {
          BT_ERR("BT PF RMIF: No callback function for connect",0,0,0);
        }
      }
      else
      {
        BT_ERR("BT PF RMIF: SCO map table full!",0,0,0);
      }
      break;
    case BT_EV_RM_CONNECTED_SCO:
      BT_MSG_DEBUG("BT PF RMIF EV RX: conn sco",0,0,0);
      sco_entry = bt_pf_rmif_lookup_by_bd_addr( &ev_msg_ptr->ev_msg.ev_rm_conns.bd_addr );
      if( sco_entry != NULL )
      {
        sco_entry->rm_handle = ev_msg_ptr->ev_msg.ev_rm_conns.handle;
        if(sco_entry->connection_complete_cb)
        {
          sco_entry->connection_complete_cb( OI_OK, 
                                             BT_HANDLE_TO_OI_HANDLE(sco_entry->rm_handle),
                                             (OI_BD_ADDR*)(&sco_entry->bd_addr) );
        }
        else
        {
          BT_ERR("BT PF RMIF: No callback function for SCO connection complete",0,0,0);
        }
      }
      else
      {
        /* How did this happen? */
        BT_ERR("BT PF RMIF: Unknown BD addr for connect.",0,0,0);
      }
      break;
    case BT_EV_RM_CONNECTION_FAILED_SCO:
      BT_MSG_DEBUG("BT PF RMIF EV RX: conn sco failed",0,0,0);
      sco_entry = bt_pf_rmif_lookup_by_bd_addr( &ev_msg_ptr->ev_msg.ev_rm_confs.bd_addr );
      if( sco_entry != NULL )
      {
        sco_entry->rm_handle = ev_msg_ptr->ev_msg.ev_rm_conns.handle;
        if(sco_entry->connection_complete_cb)
        {
          sco_entry->connection_complete_cb( bt_pf_map_event_code(ev_msg_ptr->ev_msg.ev_rm_confs.reason),
                                             BT_HANDLE_TO_OI_HANDLE(sco_entry->rm_handle),
                                             (OI_BD_ADDR*)(&sco_entry->bd_addr) );
        }
        else
        {
          BT_ERR("BT PF RMIF: No callback function for SCO connection complete",0,0,0);
        }
        bt_pf_rmif_free_db_entry( sco_entry );
      }
      else
      {
        /* How did this happen? */
        BT_ERR("BT PF RMIF: Unknown BD addr for connect failed.",0,0,0);
      }

      break;
    case BT_EV_RM_RX_DATA_SCO:
      BT_ERR("BT PF RMIF: Unhandled event: RX DATA SCO",0,0,0);
      /*      ev_msg_ptr->ev_msg.ev_rm_rxds.handle;
              ev_msg_ptr->ev_msg.ev_rm_rxds.dsm_ptr;
      */
      break;
    case BT_EV_RM_DISCONNECTED_SCO:
      BT_MSG_DEBUG("BT PF RMIF EV RX: discon sco",0,0,0);
      sco_entry = bt_pf_rmif_lookup_by_rm_handle(ev_msg_ptr->ev_msg.ev_rm_discs.handle);
      if( sco_entry != NULL )
      {
        if( sco_entry->disconnect_complete_cb )
        {
          sco_entry->disconnect_complete_cb( OI_OK,
                                             BT_HANDLE_TO_OI_HANDLE(sco_entry->rm_handle),
                                             bt_pf_map_event_code(ev_msg_ptr->ev_msg.ev_rm_discs.reason) );
        }
        else
        {
          BT_ERR("BT PF RMIF: No callback function for SCO disconnected",0,0,0);
        }
        bt_pf_rmif_free_db_entry( sco_entry );
      }
      else
      {
        /* How did this happen? */
        BT_ERR("BT PF RMIF: Unknown handle for disconnect.",0,0,0);
      }
      break;
    default:
      BT_ERR( "BT PF RMIF: unknown event type %x", 
              ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      break;

  }
}

/*===========================================================================

FUNCTION
  bt_pf_rmif_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by the Profile L2 interface.

===========================================================================*/
uint16 bt_pf_rmif_get_max_event_bytes
(
  void
)
{
  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connect_req_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connected_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connection_failed_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_rx_data_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_disconnected_sco_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );
}

/*===========================================================================

FUNCTION
  bt_pf_rmif_store_bt_event

DESCRIPTION
  Stores a specified BT event on the Profile L2 Interface BT event queue.

===========================================================================*/
LOCAL void bt_pf_rmif_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  bt_pf_store_bt_event( bt_ev_ptr, &bt_pf_rmif_ev_q_info );

}

/*===========================================================================

FUNCTION
  bt_pf_rmif_process_ev_queue

DESCRIPTION
  Dequeues events on the Profile L2 interface event queue and processes them.

===========================================================================*/
void bt_pf_rmif_process_ev_queue( void )
{

  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_pf_rmif_ev_q );

  BT_VSTAT_MAX( q_count, PF_RMIF_MAX_EV_Q_DEPTH );

  while ( (bt_ev_ptr = (bt_ev_msg_type *)q_get( &bt_pf_rmif_ev_q )) != NULL )
  {
    bt_pf_rmif_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}

/*=========================================================================*/
/*                                                                         */
/*           FUNCTIONS FOR MAPPING DEVMGR APIs                             */
/*                                                                         */
/*=========================================================================*/

/*===========================================================================

FUNCTION
  OI_DEVMGR_ScoDisconnect

DESCRIPTION
  Disconnect a SCO link
  
===========================================================================*/
OI_STATUS OI_DEVMGR_ScoDisconnect(OI_HCI_CONNECTION_HANDLE handle)
{
  OI_STATUS status;

  BT_MSG_DEBUG("BT PF RMIF: ScoDisconnect",0,0,0);

  BT_CMD( bt_cmd_rm_disconnect_sco( bt_pf_rmif_aid, 
                                    OI_HANDLE_TO_BT_HANDLE(handle),
                                    BT_RMDR_USER_ENDED ), 
          status );
  return status;
}

/*===========================================================================

FUNCTION
  OI_DEVMGR_DeregisterScoApplication

DESCRIPTION
  Deregister an application from the DevMgr.
  
===========================================================================*/
OI_STATUS OI_DEVMGR_DeregisterScoApplication(OI_DEVMGR_SCO_SIGNAL_CALLBACKS *pScoSignalCbs)
{
  BT_MSG_DEBUG("BT PF RMIF: Dereg Sco",0,0,0);

  bt_pf_rmif_free_db_entry( bt_pf_rmif_get_db_entry_by_cb( pScoSignalCbs ) );
  if( db_is_empty() )
  {
    (void)bt_cmd_rm_unregister_sco( bt_pf_rmif_aid );
    bt_pf_rmif_aid = BT_APP_ID_NULL;
  }
  
  if( pScoSignalCbs->scoConnectCompleteCb    == bt_pf_sco_default.connection_complete_cb &&
      pScoSignalCbs->scoDisconnectCompleteCb == bt_pf_sco_default.disconnect_complete_cb &&
      pScoSignalCbs->scoConnectIndCb         == bt_pf_sco_default.connect_ind_cb )
  {
    memset( &bt_pf_sco_default, 0, sizeof(bt_pf_sco_db_type) );
  }
  return OI_OK;
}

/*===========================================================================

FUNCTION
  OI_DEVMGR_RegisterScoApplication

DESCRIPTION
  Register an application from the DevMgr.
  
===========================================================================*/
OI_STATUS OI_DEVMGR_RegisterScoApplication( OI_DEVMGR_SCO_SIGNAL_CALLBACKS *pScoSignalCbs,
                                            OI_BD_ADDR                     *pAddr )
{
  OI_STATUS status = OI_OK;
  bt_pf_sco_db_type *sco_entry = NULL;

  BT_MSG_DEBUG("BT PF RMIF: Reg Sco",0,0,0);

  if( pAddr == NULL )
  {
    /* Set up default call backs */
    if( memcmp(&bt_pf_sco_default, &bt_pf_null_sco_db_entry, sizeof(bt_pf_sco_db_type)) == 0 )
    {
      sco_entry = &bt_pf_sco_default;
    }
  }
  else
  {
    sco_entry = bt_pf_rmif_lookup_by_bd_addr( (bt_bd_addr_type*)pAddr );
  }

  if( sco_entry == NULL && status == OI_OK )
  {
    if( (sco_entry = bt_pf_rmif_get_new_db_entry()) == NULL )
    {
      status = BT_CS_PF_NO_RESOURCES;
    }
  }
  else
  {
    status = BT_CS_PF_ALREADY_REGISTERED;
  }

  if(sco_entry)
  {
    sco_entry->bd_addr                = *(bt_bd_addr_type*)pAddr;
    sco_entry->connection_complete_cb = pScoSignalCbs->scoConnectCompleteCb;
    sco_entry->disconnect_complete_cb = pScoSignalCbs->scoDisconnectCompleteCb;
    sco_entry->connect_ind_cb         = pScoSignalCbs->scoConnectIndCb;
  }

  if( bt_pf_rmif_aid == BT_APP_ID_NULL )
  {
    bt_pf_rmif_aid = bt_cmd_ec_get_app_id_and_register( bt_pf_rmif_process_event );
    BT_CMD( bt_cmd_rm_register_sco( bt_pf_rmif_aid ), status );
  }

  if( sco_entry && status != OI_OK )
  {
    memset(sco_entry, 0, sizeof(bt_pf_sco_db_type));
  }

  return status;
}

/*===========================================================================

FUNCTION
  OI_DEVMGR_ScoCreateConnection

DESCRIPTION
  Create a new SCO connection
  
===========================================================================*/
OI_STATUS OI_DEVMGR_ScoCreateConnection(OI_BD_ADDR *pBdAddr)
{
  OI_STATUS status;
  
  BT_MSG_DEBUG("BT PF RMIF: Sco Create Conn",0,0,0);

  if(bt_pf_rmif_aid != BT_APP_ID_NULL )
  {
    BT_CMD( bt_cmd_rm_connect_sco( 
              bt_pf_rmif_aid, (bt_bd_addr_type*)pBdAddr, FALSE /* SCO only */ ), 
            status );
  }
  else
  {
    status = OI_STATUS_NOT_REGISTERED;
  }

  return status;
}

/*=========================================================================*/
/*                                                                         */
/*            EXTERNAL FUNCTIONS                                           */
/*                                                                         */
/*=========================================================================*/

/*===========================================================================
FUNCTION
  bt_pf_rmif_init

DESCRIPTION
  Initializes data structures. Called at boot.
===========================================================================*/
void bt_pf_rmif_init( void )
{
  (void) q_init( &bt_pf_rmif_ev_q );
  bt_pf_rmif_ev_q_info.event_q_ptr      = &bt_pf_rmif_ev_q;
  bt_pf_rmif_ev_q_info.max_q_depth      = BT_PF_RMIF_NUM_EVENTS;
  bt_pf_rmif_ev_q_info.pf_task_sig      = BT_PF_RMIF_EV_SIG;
  bt_pf_rmif_ev_q_info.max_event_bytes  = bt_pf_rmif_get_max_event_bytes();
}
#endif /* FEATURE_BT_EXTPF */
#endif /* FEATURE_BT */
