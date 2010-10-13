
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            D S  S O C K E T S   P S   T A S K   C M D   F I L E                                     

GENERAL DESCRIPTION
   This task has the cmd processing for commands sent by the sockets layer
   to PS to be processed.
   
   These commands include commands to WRITE/CONNECT/CLOSE.

  Copyright (c) 2002-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_ps.c_v   1.13   24 Feb 2003 18:32:12   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dss_ps.c#3 $ $DateTime: 2009/02/05 02:33:42 $ $Author: kbhaskar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/05/09    kk     Added NULL pointer checks in dssps_process_socket_cmd()
17/09/07    scb    Added functionality to read the 
                   NV_TCP_GRACEFUL_DORMANT_CLOSE_I nv item in dssps_init()
10/18/07    sn     Modified the handling of 'pool_id' parameter as it's data 
                   type changed.
07/18/03    ss     Get the sockfd from the socket command and validate that
                   the scb_ptr points to the correct socket. 
02/24/03    aku    Access ps_iface_ptr from scb instead of acb.
02/13/03    aku    Added support to notify sockets when DSM becomes available
                   and also fixed bug wherein sockets could not be closed 
                   once dss_pppclose() is called.
02/13/03    ss     Added socket shutdown command processing
01/24/03    usb    Removed unused variable to fix compiler warning
12/24/02    aku    Register for PS_SOCKET_PHYS_LINK_UP_EVENT_CMD and
                   PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD command handlers with
                   PS.
12/22/02    aku    IFACE_UP and IFACE_DOWN events have independent cmd 
                   handlers.
10/08/02    om     Fixed preservation of app-field across send_q.
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
10/06/02    aku    Added support for processing iface callbacks in PS task
10/02/02    mvl    fixed meta info memory leak.
09/18/02    aku    Added check for NULL ps_iface_ptr prior to calling the
                   abort function. 
09/11/02    aku    Added support for aborting protocol stack if interface is
                   configured to do so.
07/16/02    aku    Include dss_ps.h
07/14/02    na/aku Created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DS_SOCKETS
#include "assert.h"
#include "msg.h"
#include "ps_svc.h"
#include "ps_utils.h"
#include "queue.h"
#include "dsm.h"
#include "dssocki.h"
#include "dss_ps.h"
#include "dssifcbacks.h"

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
q_type ps_socket_send_q;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION DSSPS_PROCESS_SOCKET_CMD()

DESCRIPTION
  This function process the protocol packets that need to be sent by a socket
  Each item of the psi_socket_send_q is a protocol PDU and contains the 
  pointer to the socket control block. This function removes the pointer to 
  the socket control block from the enqueued item and calls the corresponding
  send routine for the socket.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: When ps_socket_send_q is empty, clear signal from set signal mask
  FALSE: Don't clear signal yet - wait for send_q to be emptied out.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssps_process_socket_cmd
( 
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
  struct sockaddr_in  ta;               /* holding destination information */
  dsm_item_type*      item_ptr;         /* ptr to item dequeued            */
  struct scb_s*       scb_ptr;          /* Ptr to socket_ctl_blk           */
  ps_meta_info_type *meta_info_ptr;     /* meta info pointer               */
  uint16 kind;                          /* kind field of enqueued item     */
  uint16 cnt = 0;                       /* Num bytes pulled up             */
  sint15 ps_errno;
  sint15 sockfd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the item from the ps_socket_send_q.
    If queue is empty then then ask for the signal to be removed.
    Note, we don't empty the queue in a while loop. Reason being that we want
    to process one write cmd for socket and then give a chance for a packet
    to be read thus balancing the flows.
  -------------------------------------------------------------------------*/
  item_ptr = (dsm_item_type *)q_get( &ps_socket_send_q);
  if( item_ptr == NULL)
  {
    return( TRUE);
  }

  kind      = item_ptr->kind;
    
  /*-------------------------------------------------------------------------
    Make sure that if the item app_field indicates that there is meta
    information that it is gotten so it can be passed to the write function
    directly, otherwise, set the meta_info_ptr to NULL.
  -------------------------------------------------------------------------*/
  if(item_ptr->app_field != 0)
  {
    /*-----------------------------------------------------------------------
      Get the meta information from the app_field, and set the app_field to 0
      to ensure that it is not miss understood by someone else.
    -----------------------------------------------------------------------*/
    meta_info_ptr = (ps_meta_info_type*)item_ptr->app_field;
  }
  else
  {
    meta_info_ptr = NULL;
  }

  /*-------------------------------------------------------------------------
    Get the socket descriptor.
  -------------------------------------------------------------------------*/
  cnt = dsm_pullup( &item_ptr, &sockfd, sizeof(sockfd));
  if (cnt != sizeof(sockfd))
  {
    MSG_ERROR("Pullup ERR: discarding", 0, 0, 0);
    dsm_free_packet( &item_ptr);
    PS_META_INFO_FREE( &meta_info_ptr );
    return( FALSE);
  }

  /*-------------------------------------------------------------------------
    Get the socket control block.
  -------------------------------------------------------------------------*/
  cnt = dsm_pullup( &item_ptr, &scb_ptr, sizeof(scb_ptr));
  if (cnt != sizeof(scb_ptr))
  {
    MSG_ERROR("Pullup ERR: discarding", 0, 0, 0);
    dsm_free_packet( &item_ptr);
    PS_META_INFO_FREE( &meta_info_ptr );
    return( FALSE);
  }
  if ( scb_ptr->fcn_ptr_table == NULL )
  {
    MSG_ERROR("Empty function pointer table", 0, 0, 0);
    dsm_free_packet( &item_ptr);
    PS_META_INFO_FREE( &meta_info_ptr );
    return( FALSE);
  }

  /*-------------------------------------------------------------------------
    Make sure that the socket has not been freed.
  -------------------------------------------------------------------------*/
  if ( scb_ptr->sockfd != sockfd )
  {
    MSG_MED("Freeing dsm item because socket closed, oldfd=%d, newfd=%d", 
             sockfd, scb_ptr->sockfd, 0);
    dsm_free_packet( &item_ptr);
    PS_META_INFO_FREE( &meta_info_ptr );
    return( FALSE);
  }

  /*-------------------------------------------------------------------------
    Get the destination socket address structure. The structure is pushed 
    onto the dsm_item_type chain by dssocki_send_to_ps (). 
  -------------------------------------------------------------------------*/
  cnt = dsm_pullup( &item_ptr, &ta, sizeof(struct sockaddr_in));
  if (cnt != sizeof(struct sockaddr_in))
  {
    MSG_ERROR("Sock%d pullup ERR: discarding", scb_ptr->sockfd, 0, 0);
    dsm_free_packet( &item_ptr);
    PS_META_INFO_FREE( &meta_info_ptr );
    return( FALSE);
  }
  
  /*-------------------------------------------------------------------------
     Restore the original payload's app-field to zero.
  -------------------------------------------------------------------------*/  
  item_ptr->app_field = 0;

  /*-------------------------------------------------------------------------
     Call protocol specific send() function if this is a payload item.
     If this is a connect msg, call the protocol specific connect function.
     If this is a close command, call the protocol specific close function.
  -------------------------------------------------------------------------*/
  if ( kind == DSM_PS_SOCKET_PAYLOAD)
  {
    MSG_LOW("Sock%d WRITE", scb_ptr->sockfd, 0, 0);
    scb_ptr->fcn_ptr_table->write( scb_ptr->protocol_ctl_blk, 
                                   &ta, 
                                   item_ptr, 
                                   meta_info_ptr
                                 );
  }
  else if ( kind == DSM_PS_SOCKET_CONNECT)
  {
    MSG_MED("Sock%d CONNECTing", scb_ptr->sockfd, 0, 0);
    scb_ptr->fcn_ptr_table->connect( scb_ptr );
    /* item_ptr needs to be freed after we connect */
    dsm_free_packet(&item_ptr);
    PS_META_INFO_FREE(&meta_info_ptr);
  }
  else if ( kind == DSM_PS_SOCKET_CLOSE)
  {

    /*-----------------------------------------------------------------------
      If the interface is down (and dormant) and the interface has been 
      configured to not close protocol stack gracefully under these 
      conditions, abort the protocol stack, else close it gracefully.
      Also check for NULL routing cache in the scb.
    -----------------------------------------------------------------------*/
      if((scb_ptr->fcn_ptr_table != NULL) &&
         (scb_ptr->fcn_ptr_table->abort != NULL) &&
         ((scb_ptr->routing_cache == NULL) ||
          (((ps_iface_phys_link_state(scb_ptr->routing_cache) == 
                                                           PHYS_LINK_DOWN) ||
            (ps_iface_phys_link_state(scb_ptr->routing_cache) == 
                                                    PHYS_LINK_GOING_DOWN)) &&
           (scb_ptr->routing_cache->graceful_dormant_close == 
                                                                    FALSE))))
      {
        scb_ptr->fcn_ptr_table->abort(scb_ptr, &ps_errno);
      }
    /*-----------------------------------------------------------------------
     Not dormant, perform the protocol specific operation.
    -----------------------------------------------------------------------*/
    else if (scb_ptr->fcn_ptr_table != NULL)
    {  
      scb_ptr->fcn_ptr_table->close( scb_ptr, &ps_errno );
    }
    dsm_free_packet(&item_ptr);
    PS_META_INFO_FREE(&meta_info_ptr);
  }
  else if ( kind == DSM_PS_SOCKET_SHUTDOWN )
  {
    if ( scb_ptr->fcn_ptr_table != NULL &&
         scb_ptr->fcn_ptr_table->shutdown != NULL )
    {
      scb_ptr->fcn_ptr_table->shutdown( scb_ptr, item_ptr, &ps_errno );
    }
  }
  else if ( kind == DSM_PS_SOCKET_LISTEN)
  {
    scb_ptr->fcn_ptr_table->listen( scb_ptr );
    /* item_ptr needs to be freed after we connect */
    dsm_free_packet(&item_ptr);
    PS_META_INFO_FREE(&meta_info_ptr);
  }
  else
  {
    ASSERT( 0);
  }

  return( FALSE);

} /* dssps_process_socket_cmd() */

/*===========================================================================
FUNCTION DSS_MEM_EVENT_CTRL()

DESCRIPTION
  This function notify sockets apps when DSM memory is available.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dss_mem_event_ctrl
(
  dsm_mempool_id_enum_type pool_id,                      /* memory pool ID */
  dsm_mem_level_enum_type event,                   /* type of memory event */
  dsm_mem_op_enum_type op
)
{
  static uint32 dsm_mem_mask=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pool_id == DSM_DS_SMALL_ITEM_POOL)
  {
      switch (event)
      {
        case DSM_MEM_LEVEL_MANY:
          /*-----------------------------------------------------------------
            Notify sockets applications if the mobile has previously run out
            of both large and small items.
          -----------------------------------------------------------------*/
          MSG_LOW(" SMALL ITEM MANY ITEMS MARK Reached",0,0,0);
          if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK | 
                              DS_FLOW_DSM_LARGE_Q_MASK))
          {
            MSG_MED("Small items available, mask=0x%x",dsm_mem_mask,0,0);
            dssocki_socket_event_occurred(NULL, NULL);
          }
          dsm_mem_mask &= ~(DS_FLOW_DSM_SMALL_Q_MASK);
          break;

        case DSM_MEM_LEVEL_FEW:
          /*-----------------------------------------------------------------
            Update the static mask.
          -----------------------------------------------------------------*/
          MSG_LOW(" SMALL ITEM FEW ITEMS MARK Reached",0,0,0);
          dsm_mem_mask |= DS_FLOW_DSM_SMALL_Q_MASK;
          break;

        default:
          MSG_LOW("Do nothing",0,0,0);
          break;
      }
  }
  else if(pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
      switch (event)
      {
        case DSM_MEM_LEVEL_MANY:
          /*-----------------------------------------------------------------
            Notify sockets applications if the mobile has previously run out
            of both large and small items.            
          -----------------------------------------------------------------*/
          MSG_LOW(" LARGE ITEM MANY ITEMS MARK Reached",0,0,0);
          if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK | 
                              DS_FLOW_DSM_LARGE_Q_MASK))
          {
            MSG_MED("Large items available, mask=0x%x",dsm_mem_mask,0,0);
            dssocki_socket_event_occurred(NULL, NULL);
          }
          dsm_mem_mask &= ~(DS_FLOW_DSM_LARGE_Q_MASK);
          break;

        case DSM_MEM_LEVEL_FEW:
          /*-----------------------------------------------------------------
            Update the static mask.
          -----------------------------------------------------------------*/
          MSG_LOW(" LARGE ITEM FEW ITEMS MARK Reached",0,0,0);
          dsm_mem_mask |= DS_FLOW_DSM_LARGE_Q_MASK;
          break;
        
        default:
          MSG_LOW("Do nothing",0,0,0);
          break;
      }
  }
  else 
  {
    MSG_LOW("Do nothing", 0, 0, 0);
  }
} /* dss_mem_event_ctrl() */

/*===========================================================================
FUNCTION DSSPS_NV_INIT()

DESCRIPTION
  Initialization of socket configuration via NV/RUIM in the PS task.

DEPENDENCIES
  Called either during powerup or when the SIM_INIT_COMPLETED event
  is received.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_nv_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*-------------------------------------------------------------------------
   * Retrieve the value of tcp_graceful_dormant_close flag from the NV
  -------------------------------------------------------------------------*/
  ps_nv_status = ps_get_nv_item(NV_TCP_GRACEFUL_DORMANT_CLOSE_I,&ps_nv_item);
  if( ps_nv_status != NV_DONE_S )
  {
    MSG_HIGH("TCP graceful dormant close flag unavailable, using default",
             0, 0, 0);
    /*-----------------------------------------------------------------------
     * set the global variable with default value and then set the NV item
     * to default value
     ----------------------------------------------------------------------*/
    sock_config_cb.tcp_graceful_dormant_close = FALSE;
    ps_nv_item.tcp_graceful_dormant_close = FALSE;
    ps_nv_status = 
                ps_put_nv_item(NV_TCP_GRACEFUL_DORMANT_CLOSE_I,&ps_nv_item);
    if(ps_nv_status != NV_DONE_S)
    {
      MSG_HIGH("Unable to set TCP dormant close flag to default value",
	       0,0,0);
    }
  }
  else
  {
     sock_config_cb.tcp_graceful_dormant_close = 
       ps_nv_item.tcp_graceful_dormant_close;
  }
}
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DSSPS_INIT()

DESCRIPTION
  Powerup initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at powerup from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_init
( 
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void)dssps_nv_init();
  /*-------------------------------------------------------------------------
    Initialize the Socket queue that carries commands from the app tasks to
    PS.
  -------------------------------------------------------------------------*/
  (void)q_init( &ps_socket_send_q);

  /*-------------------------------------------------------------------------
    Register command handler for socket iface up/down and IP address changed
    events.
  -------------------------------------------------------------------------*/
  ps_set_cmd_handler(PS_SOCKET_NETWORK_UP_EVENT_CMD, 
                     dssocki_process_iface_up_ev_cback);
  ps_set_cmd_handler(PS_SOCKET_NETWORK_DOWN_EVENT_CMD, 
                     dssocki_process_iface_down_ev_cback);
  ps_set_cmd_handler(PS_SOCKET_IFACE_ADDR_CHANGED_CMD, 
                     dssifcb_ip_addr_changed_cback);
  ps_set_cmd_handler(PS_SOCKET_PHYS_LINK_UP_EVENT_CMD, 
                     dssocki_process_phys_link_up_ev_cback);
  ps_set_cmd_handler(PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD, 
                     dssocki_process_phys_link_down_ev_cback);

  /*-------------------------------------------------------------------------
    Register handlers and enable signals used for Sockets.
  -------------------------------------------------------------------------*/
  ps_set_sig_handler(PS_SOCKET_RX_Q_SIGNAL, dssps_process_socket_cmd, NULL); 
  ps_enable_sig( PS_SOCKET_RX_Q_SIGNAL);
  ps_set_sig_handler(PS_SOCKET_FLOW_ENABLED_SIG, 
                     dssifcb_process_flow_enabled_cback, 
                     NULL); 
  ps_enable_sig( PS_SOCKET_FLOW_ENABLED_SIG);

  /*-------------------------------------------------------------------------
    Register for DSM memory events (both SMALL and LARGE items.
  -------------------------------------------------------------------------*/
  (void) dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                                DSM_MEM_LEVEL_MANY,
                                DSM_MEM_OP_FREE,
                                dss_mem_event_ctrl );

  (void) dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                                DSM_MEM_LEVEL_FEW,
                                DSM_MEM_OP_NEW,
                                dss_mem_event_ctrl );

  (void) dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                                DSM_MEM_LEVEL_MANY,
                                DSM_MEM_OP_FREE,
                                dss_mem_event_ctrl );

  (void) dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                                DSM_MEM_LEVEL_FEW,
                                DSM_MEM_OP_NEW,
                                dss_mem_event_ctrl );

} /* dssps_init() */

#endif /* FEATURE_DS_SOCKETS */
